/*
 * Object File is designed to contain all labels, all keys and some values (those that need more than 56 bits to be
 * encoded). Strings are meant to be encoded in UTF-8.
 *
 * To store an object, the ObjectFile will receive the bytes from the object and it will append to the file the
 * length in bytes followed by the actual bytes. The position where the length was written is returned so
 * the object can be readed later using that position. The 64 bits ID of a graph element that represent an object
 * contains the position encoded in the last 56 bits (and first 8 bits are used to determine the type).
 *
 * Because the ID=0 is special (represents a non-existent object), we need to write a trash byte when creating the
 * file so the first object will have the ID=1.
 * */

#ifndef STORAGE__OBJECT_FILE_H_
#define STORAGE__OBJECT_FILE_H_

#include "base/graph/value/value.h"
#include "base/ids/object_id.h"

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Node;

class ObjectFile
{
    public:
        ObjectFile(const std::string& filename);
        ~ObjectFile() = default;
        std::unique_ptr<std::vector<unsigned char>> read(uint64_t id);
        uint64_t write(std::vector<unsigned char>& bytes);

    private:
        std::fstream& file;
};

#endif // STORAGE__OBJECT_FILE_H_
