/* We use a FileId outside of FileManager as a unique identifier to the file we need to use,
 * because storing the filename string as a value use more memory than needed; and storing a
 * reference to a string can be problematic when destructors are called at the end of the execution.
 */

#ifndef STORAGE__FILE_ID_H_
#define STORAGE__FILE_ID_H_

#include <cstdint>

class FileId {
public:
    static constexpr uint_fast32_t UNASSIGNED = UINT32_MAX;

    uint_fast32_t id;

    FileId(uint_fast32_t id)
        : id(id) {}

    bool operator<(const FileId other) const {
        return this->id < other.id;
    }
};

#endif // STORAGE__FILE_ID_H_
