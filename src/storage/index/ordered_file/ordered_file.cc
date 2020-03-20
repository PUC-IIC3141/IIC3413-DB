#include "ordered_file.h"

#include "storage/index/record.h"
#include "storage/file_manager.h"

#include <chrono>
#include <climits>
#include <iostream>
#include <memory>

#define TUPLES_PER_BLOCK 4096/8
#define MAX_RUNS 8

using namespace std;

OrderedFile::OrderedFile(const string& filename, uint_fast8_t tuple_size)
    : tuple_size(tuple_size),
      file_id(file_manager.get_file_id(filename)),
      tmp_file_id(file_manager.get_file_id(filename + ".tmp")),
      file(file_manager.get_file(file_id)),
      tmp_file(file_manager.get_file(tmp_file_id)),
      bytes_per_tuple(sizeof(uint64_t)*tuple_size),
      block_size_in_bytes(TUPLES_PER_BLOCK*bytes_per_tuple)
{
    big_buffer = new uint64_t[(MAX_RUNS+1)*TUPLES_PER_BLOCK*tuple_size];
    buffer = new uint64_t*[MAX_RUNS];

    output_buffer = &big_buffer[0];
    for (uint_fast32_t i = 0; i < MAX_RUNS; i++) {
        buffer[i] = &big_buffer[(i+1)*TUPLES_PER_BLOCK*tuple_size];
    }
    current_output_pos = 0;
}


OrderedFile::~OrderedFile() {
    delete[] buffer;
    delete[] big_buffer;
    file_manager.close(file_id);
    file_manager.remove(file_id);
}

void OrderedFile::begin_iter() {
    file.seekg(0, ios::end);
    filesize = file.tellg();
    file.seekg(0, ios::beg);
}

bool OrderedFile::has_more_tuples() {
    return file.tellg() < filesize;
}

uint_fast32_t OrderedFile::next_tuples(uint64_t* output, uint_fast32_t max_tuples) {
    file.read((char*)output, max_tuples*bytes_per_tuple);
    auto res = file.gcount()/bytes_per_tuple;
    file.clear(); // clear posible badbit

    return res;
}

void OrderedFile::append_record(const Record& record) {

    for (int col = 0; col < tuple_size; col++) {
        output_buffer[current_output_pos*tuple_size+col] = record.ids[col];
    }
    current_output_pos++;
    if (current_output_pos == TUPLES_PER_BLOCK) {
        file.seekg(0, ios::end);
        file.write((char*) output_buffer, block_size_in_bytes);
        current_output_pos = 0;
    }
}

void OrderedFile::order(vector<uint_fast8_t> column_order) {
    file_manager.ensure_open(tmp_file_id);

    if (current_output_pos > 0) {
        file.write((char*) output_buffer, bytes_per_tuple*current_output_pos);
        current_output_pos = 0;
    }

    // auto start = std::chrono::system_clock::now();
    bool reading_orginal_file = true;

    file.seekg(0, ios::end);
    const uint64_t size_in_bytes = file.tellg();
    uint_fast32_t total_blocks = size_in_bytes/block_size_in_bytes + (size_in_bytes%block_size_in_bytes != 0);

    bool reorder = false;
    for (size_t i = 0; i < column_order.size(); i++) {
        if (column_order[i] != i) {
            reorder = true;
            break;
        }
    }

    // Order chunks of size (MAX_RUNS) blocks
    for (uint_fast32_t i = 0; i < total_blocks; /*i+= MAX_RUNS*/i++) {
        create_run(big_buffer, i, column_order, reorder);
    }

    // auto end_phase0 = std::chrono::system_clock::now();
    // std::chrono::duration<float,std::milli> duration = end_phase0 - start;
    // std::cout << duration.count() << "ms " << std::endl;

    uint_fast32_t buffer_size[MAX_RUNS];
    uint_fast32_t buffer_current_pos[MAX_RUNS];
    uint_fast32_t buffer_current_block[MAX_RUNS];

    // MERGE ITERATION
    for (uint_fast32_t merge_size = 1; merge_size < total_blocks; merge_size *= MAX_RUNS) {
        file.seekg(0, ios::beg);
        tmp_file.seekg(0, ios::beg);
        std::fstream& file_reading = reading_orginal_file? file : tmp_file;
        std::fstream& file_writing = reading_orginal_file? tmp_file : file;

        for (uint_fast32_t i = 0; i < total_blocks; i+= merge_size*MAX_RUNS) {
            file_reading.seekg(i*block_size_in_bytes, ios::beg);
            // SET HOW MANY RUNS ARE NEEDED
            uint_fast32_t runs = (total_blocks < (i + (merge_size*MAX_RUNS))) ?
                ((total_blocks - i)/merge_size + ((total_blocks - i)%merge_size != 0)) :
                MAX_RUNS;

            // FILL BUFFERS
            for (uint_fast32_t run = 0; run < runs; run++) {
                buffer_current_pos[run] = 0;
                buffer_current_block[run] = 0;
                file_reading.seekg((i + (run*merge_size)) * block_size_in_bytes, ios::beg);
                file_reading.read((char*)buffer[run], block_size_in_bytes);
                buffer_size[run] = file_reading.gcount()/bytes_per_tuple;
                file_reading.clear(); // clear posible badbit
            }

            // merge runs
            while (true) {
                int min_run = -1;
                for (uint_fast32_t run = 0; run < runs; run++) {
                    if (buffer_current_pos[run] < buffer_size[run]) { // check run is not empty
                        if (min_run == -1) {
                            min_run = run;
                        } else {
                            // compare current record at min_run vs record at run
                            for (uint_fast8_t col = 0; col < tuple_size; col++/*uint_fast8_t col : column_order*/) {
                                if (buffer[run][buffer_current_pos[run]*tuple_size+col] < buffer[min_run][buffer_current_pos[min_run]*tuple_size+col]) {
                                    min_run = run;
                                    break;
                                } else if (buffer[run][buffer_current_pos[run]*tuple_size+col] > buffer[min_run][buffer_current_pos[min_run]*tuple_size+col]) {
                                    break;
                                }
                            }
                        }
                    }
                }
                if (min_run == -1) {
                    break;
                }
                // write current min_record to buffer
                for (int col = 0; col < tuple_size; col++) {
                    output_buffer[current_output_pos*tuple_size+col] = buffer[min_run][buffer_current_pos[min_run]*tuple_size+col];
                }
                current_output_pos++;
                buffer_current_pos[min_run]++;

                // if output_buffer is full, write to file
                if (current_output_pos == TUPLES_PER_BLOCK) {
                    current_output_pos = 0;
                    file_writing.write((char*) output_buffer, block_size_in_bytes);
                }

                // check if buffer[min_run] is empty
                if (buffer_current_pos[min_run] == buffer_size[min_run]) {
                    buffer_current_block[min_run]++;
                    if (buffer_current_block[min_run] < merge_size) { // if run has another block
                        buffer_current_pos[min_run] = 0;
                        file_reading.seekg((i + (min_run*merge_size) + buffer_current_block[min_run]) * block_size_in_bytes, ios::beg);
                        file_reading.read((char*)buffer[min_run], block_size_in_bytes);
                        buffer_size[min_run] = file_reading.gcount()/bytes_per_tuple;
                        file_reading.clear(); // clear posible badbit
                    }
                }
            }
            // last block is not written unless it was full at last iteration
            if (current_output_pos > 0) {
                file_writing.write((char*) output_buffer, current_output_pos*bytes_per_tuple);
                current_output_pos = 0;
            }
        }
        reading_orginal_file = !reading_orginal_file;
    }

    if (!reading_orginal_file) {
        file_manager.remove(file_id);
        file_manager.rename(tmp_file_id, file_id);
    }
    else {
        file_manager.remove(tmp_file_id);
    }
    file_manager.ensure_open(file_id);

    // auto end = std::chrono::system_clock::now();
    // duration = end - end_phase0;
    // std::cout << duration.count() << "ms " << std::endl;
}

// First Step: order (MAX_RUNS) blocks at once
void OrderedFile::create_run(uint64_t* buffer, uint_fast32_t block_number, vector<uint_fast8_t>& column_order, bool reorder)
{
    file.seekg(block_number*block_size_in_bytes, ios::beg);
    file.read((char*)buffer, block_size_in_bytes/**MAX_RUNS*/);
    uint_fast32_t bytes_readed = file.gcount();
    uint_fast32_t tuples = bytes_readed / bytes_per_tuple;
    file.clear(); // clear posible badbit

    uint64_t* key = new uint64_t[tuple_size];
    if (reorder) {
        // reorder according to column_order
        for (uint_fast32_t i = 0; i < tuples; i++) {
            for (uint_fast8_t n = 0; n < tuple_size; n++) {
                key[column_order[n]] = buffer[i*tuple_size + n];
            }
            for (uint_fast8_t n = 0; n < tuple_size; n++) {
                buffer[i*tuple_size + n] = key[n];
            }
        }
    }

    // insertion sort
    for (uint_fast32_t i = 1; i < tuples; i++) {
        for (uint_fast8_t n = 0; n < tuple_size; n++) {
            key[n] = buffer[i*tuple_size + n];
        }
        uint_fast32_t j = i - 1;
        while (record_less_than(j, key, buffer))
        {
            move_record_right(j, buffer);
            if (j == 0) { // necesary to check this because j is unsigned
                j--;
                break;
            }
            j--;
        }
        // insert key at j+1
        assign_record(key, j+1, buffer);//arr[j + 1] = key;
    }
    file.seekg(block_number*block_size_in_bytes, ios::beg);
    file.write((char*)buffer, bytes_readed);
    delete[] key;
}

// returns true if key is less than the record in the buffer at buffer_pos
bool OrderedFile::record_less_than(uint_fast32_t buffer_pos, uint64_t* key, uint64_t* buffer)
{
    for (uint_fast8_t col = 0; col < tuple_size; col++) {
        if (key[col] < buffer[buffer_pos*tuple_size + col]) {
            return true;
        }
        else if (key[col] > buffer[buffer_pos*tuple_size + col]) {
            return false;
        }
    }
    return false;// they are equal
}

void OrderedFile::move_record_right(uint_fast32_t buffer_pos, uint64_t* buffer)
{
    for (uint_fast8_t i = 0; i < tuple_size; i++) {
        buffer[(buffer_pos+1)*tuple_size + i] = buffer[buffer_pos*tuple_size + i];
    }
}

void OrderedFile::assign_record(uint64_t* key, uint_fast32_t buffer_pos, uint64_t* buffer)
{
    for (uint_fast8_t i = 0; i < tuple_size; i++) {
        buffer[buffer_pos*tuple_size + i] = key[i];
    }
}

void OrderedFile::print()
{
    std::cout << "printing\n";
    file.seekg(0, ios::beg);
    int count = 1;

    uint64_t* buffer = new uint64_t[tuple_size];
    file.read((char*)buffer, bytes_per_tuple);
    auto a = file.gcount();
    while (a) {
        std::cout << count++;
        std::cout << " " << a;
        for (int i = 0; i < tuple_size; i++) {
            std::cout << "\t" << buffer[i];
        }
        std::cout << "\n";
        file.read((char*)buffer, bytes_per_tuple);
        a = file.gcount();
    }
    file.clear(); // clear badbit?
    delete[] buffer;
}

void OrderedFile::check_order(vector<uint_fast8_t> column_order)
{
    std::cout << "checking...\n";
    file.seekg(0, ios::beg);

    uint64_t* buffer = new uint64_t[tuple_size];
    file.read((char*)buffer, bytes_per_tuple);

    std::vector<uint64_t> arrA;
    for (auto& col : column_order) {
        arrA.push_back(buffer[col]);
    }

    Record recordA = Record(arrA);

    file.read((char*)buffer, bytes_per_tuple);
    auto a = file.gcount();
    std::vector<uint64_t> arrB;
    for (auto& col : column_order) {
        arrB.push_back(buffer[col]);
    }

    Record recordB = Record(arrB);

    int count = 2;
    while (a) {
        if (!(recordA < recordB)) {
            cout << "Bad ordering at tuple " << count << "\n";
            for (int i = 0; i < tuple_size; i++) {
                cout << recordA.ids[i] << "\t";
            }
            cout << " > ";
            for (int i = 0; i < tuple_size; i++) {
                cout << recordB.ids[i] << "\t";
            }
            cout << "\n";
            // exit(-1);
        }
        recordA = recordB;

        file.read((char*)buffer, bytes_per_tuple);
        a = file.gcount();
        std::vector<uint64_t> arr;
        arr.clear();
        for (auto& col : column_order) {
            arr.push_back(buffer[col]);
        }
        recordB = Record(arr);
        count++;
    }
    file.clear(); // clear badbit?
    delete[] buffer;
}