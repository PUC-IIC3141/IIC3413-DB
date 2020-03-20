#include "object_file.h"

#include <iostream>

#include "storage/file_manager.h"

using namespace std;

ObjectFile::ObjectFile(const string& filename)
    : file(file_manager.get_file(file_manager.get_file_id(filename)))
{
    file.seekg (0, file.end);
    // If the file is empty, write a trash byte to prevent the ID = 0
    if (file.tellg() == 0) {
        char c = 0;
        file.write(&c, 1);
    }
}


unique_ptr<vector<unsigned char>> ObjectFile::read(uint64_t id) {
    // set position=`id` and read the length of the object
    file.seekg(id);
    char length_b[4]; // 4 bytes to store the length of the object
    file.read(length_b, 4);
    uint32_t length = *(uint32_t *)length_b;

    // read the following `length` bytes
    auto value = make_unique<vector<unsigned char>>(length);

    // check sanity
    if (file.eof()) {
        cout << "OBJECT FILE ERROR: tried to read inexistent object.\n";
        file.clear(); // important to clear, otherwise following calls to this method will throw same error.
        return value;
    }

    file.read((char*) value->data(), length);
    return value;
}


uint64_t ObjectFile::write(vector<unsigned char>& bytes) {
    // set position at end of file
    file.seekg(0, file.end);

    // remember the actual position where we are writing the object
    uint64_t write_pos = file.tellg();

    // write to the file the length in bytes
    uint32_t length = bytes.size();
    file.write((const char *)&length, 4);

    // write to the file the bytes of the object
    file.write((const char*)bytes.data(), length);

    return write_pos;
}
