#include "file_manager.h"

#include "storage/file_id.h"
#include "storage/page.h"
#include "storage/buffer_manager.h"

#include <algorithm>
#include <cstdio>
#include <experimental/filesystem>
#include <iostream>
#include <new>         // placement new
#include <type_traits> // aligned_storage

using namespace std;

static int nifty_counter; // zero initialized at load time
static typename std::aligned_storage<sizeof (FileManager), alignof (FileManager)>::type
    file_manager_buf; // memory for the object
FileManager& file_manager = reinterpret_cast<FileManager&> (file_manager_buf);

FileManager::FileManager() {
}


FileManager::~FileManager() {
    cout << "~FileManager()\n";

    buffer_manager.flush();
    for (auto file : opened_files) {
        if (file->is_open()) {
            file->close();
        }
    }
}


void FileManager::ensure_open(FileId file_id) {
    if (!opened_files[file_id.id]->is_open()) {
        if (!experimental::filesystem::exists(filenames[file_id.id])) {
            // `ios::app` creates the file if it doesn't exists but we don't want it open in append mode,
            // so we close it and open it again without append mode
            opened_files[file_id.id]->open(filenames[file_id.id], ios::out|ios::app);
            opened_files[file_id.id]->close();
        }
        opened_files[file_id.id]->open(filenames[file_id.id], ios::in|ios::out|ios::binary);
    }
}


void FileManager::close(FileId file_id) {
    opened_files[file_id.id]->close();
}


void FileManager::remove(FileId file_id) {
    close(file_id);
    std::remove(filenames[file_id.id].c_str());
}


void FileManager::rename(FileId old_name_id, FileId new_name_id) {
    close(old_name_id);
    close(new_name_id);

    experimental::filesystem::rename(filenames[old_name_id.id], filenames[new_name_id.id]);
}


uint_fast32_t FileManager::count_pages(FileId file_id) {
    return experimental::filesystem::file_size(filenames[file_id.id])/PAGE_SIZE;
}


string FileManager::get_filename(FileId file_id) {
    return filenames[file_id.id];
}


void FileManager::flush(PageId page_id, char* bytes) {
    fstream& file = get_file(page_id.file_id);
    file.seekp(page_id.page_number*PAGE_SIZE);
    file.write(bytes, PAGE_SIZE);
}


void FileManager::read_page(PageId page_id, char* bytes) {
    fstream& file = get_file(page_id.file_id);
    file.seekg(0, file.end);
    uint_fast32_t file_pos = file.tellg();
    if (file_pos/PAGE_SIZE <= page_id.page_number) {   // new file block
        for (int i = 0; i < PAGE_SIZE; i++) {
            bytes[i] = 0;
        }
        file.write(bytes, PAGE_SIZE);
    }
    else {                                     // existing file block
        file.seekg(page_id.page_number*PAGE_SIZE);
        file.read(bytes, PAGE_SIZE);
    }
}


fstream& FileManager::get_file(FileId file_id) {
    ensure_open(file_id);
    return *opened_files[file_id.id];
}


FileId FileManager::get_file_id(const string& filename) {
    string file_path = "test_files/" + filename; // TODO: get folder path by config and/or graph?
    for (size_t i = 0; i < filenames.size(); i++) {
        if (file_path.compare(filenames[i]) == 0) {
            return FileId(i);
        }
    }

    filenames.push_back(file_path);
    fstream* file = new fstream();
    if (!experimental::filesystem::exists(file_path)) {
        file->open(file_path, ios::out|ios::app);
        file->close();
    }
    file->open(file_path, ios::in|ios::out|ios::binary);
    opened_files.push_back(file);

    return FileId(filenames.size()-1);
}


// Nifty counter trick
FileManagerInitializer::FileManagerInitializer() {
    if (nifty_counter++ == 0) new (&file_manager) FileManager(); // placement new
}

FileManagerInitializer::~FileManagerInitializer() {
    if (--nifty_counter == 0) (&file_manager)->~FileManager();
}
