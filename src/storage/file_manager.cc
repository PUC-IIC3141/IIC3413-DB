#include "file_manager.h"

#include <algorithm>
#include <cstdio>
#include <experimental/filesystem>
#include <iostream>
#include <new>         // placement new
#include <type_traits> // aligned_storage

#include "storage/file_id.h"
#include "storage/page.h"

using namespace std;

// memory for the object
static typename std::aligned_storage<sizeof(FileManager), alignof(FileManager)>::type file_manager_buf;
// global object
FileManager& file_manager = reinterpret_cast<FileManager&>(file_manager_buf);


FileManager::FileManager(std::string _db_folder) :
    db_folder(_db_folder)
{
    if (experimental::filesystem::exists(db_folder)) {
        if (!experimental::filesystem::is_directory(db_folder)) {
            throw std::runtime_error("Cannot create database directory: \"" + db_folder +
                                     "\", a file with that name already exists.");
        }
    }
    else {
        experimental::filesystem::create_directories(db_folder);
    }
}


FileManager::~FileManager() {
}


void FileManager::init(std::string db_folder) {
    new (&file_manager) FileManager(db_folder); // placement new
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
    // TODO: integrar buffer manager
    close(file_id);
    std::remove(filenames[file_id.id].c_str());
}


void FileManager::rename(FileId old_name_id, FileId new_name_id) {
    // TODO: integrar buffer manager
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
    string file_path = db_folder + "/" + filename;
    // TODO: si el modelo cambiara y se necesitaran tener muchos archivos
    // distintos, hay que cambiar la busqueda para que sea O(log n)
    for (size_t i = 0; i < filenames.size(); i++) {
        if (file_path.compare(filenames[i]) == 0) {
            return FileId(i);
        }
    }

    filenames.push_back(file_path);
    auto file = make_unique<fstream>();
    if (!experimental::filesystem::exists(file_path)) {
        file->open(file_path, ios::out|ios::app);
        if (file->fail()) {
            throw std::runtime_error("Could not open file " + file_path);
        }
        file->close();
    }
    file->open(file_path, ios::in|ios::out|ios::binary);
    opened_files.push_back(move(file));

    return FileId(filenames.size()-1);
}
