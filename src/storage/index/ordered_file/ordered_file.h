/*
 * OrderedFile allows inserting a large number of records and then sort them in
 * ascending order. It also allows sorting by changing the order of the columns,
 * which is useful for creating different B+Tree permutations.
 * The ordering is perfomed using blocks, each block will be ordered using
 * insertion sort and then will be merged until the complete file is ordered.
 * Merging phase uses a temporary file to store the result.
 * */

#ifndef STORAGE__ORDERED_FILE_H_
#define STORAGE__ORDERED_FILE_H_

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "storage/file_id.h"

class Record;

class OrderedFile {
public:
    const uint_fast8_t tuple_size;

    OrderedFile(const std::string& filename, uint_fast8_t tuple_size);
    ~OrderedFile();

    void append_record(const Record& record);
    void order(std::vector<uint_fast8_t> column_order);

    void print();
    void check_order(std::vector<uint_fast8_t> column_order);

    void begin_iter();
    bool has_more_tuples();
    uint_fast32_t next_tuples(uint64_t* output, uint_fast32_t max_tuples);

private:
    FileId file_id;
    FileId tmp_file_id;
    std::fstream& file;
    std::fstream& tmp_file;

    const uint_fast8_t bytes_per_tuple;
    const uint_fast32_t block_size_in_bytes;
    uint_fast32_t current_output_pos;
    uint64_t* output_buffer;
    uint64_t* big_buffer;
    uint64_t** buffer;
    long filesize;

    void create_run(uint64_t* buffer, uint_fast32_t block_number, std::vector<uint_fast8_t>& column_order, bool reorder);
    bool record_less_than(uint_fast32_t buffer_pos, uint64_t* key, uint64_t* buffer);
    void move_record_right(uint_fast32_t buffer_pos, uint64_t* buffer);
    void assign_record(uint64_t* key, uint_fast32_t buffer_pos, uint64_t* buffer);
};

#endif // STORAGE__ORDERED_FILE_H_
