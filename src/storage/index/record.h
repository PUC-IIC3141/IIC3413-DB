#ifndef STORAGE__RECORD_H_
#define STORAGE__RECORD_H_

#include <cstdint>
#include <ostream>
#include <vector>

class Record {
public:
    std::vector<uint64_t> ids;

    Record();
    Record(std::vector<uint64_t> arr);
    Record(uint64_t e1);
    Record(uint64_t e1, uint64_t e2);
    Record(uint64_t e1, uint64_t e2, uint64_t e3);

    Record(const Record& copy);
    ~Record();

    void operator=(const Record& other);
    bool operator<(const Record& other);
    bool operator<=(const Record& other);

    friend std::ostream& operator<<(std::ostream& os, const Record& record);
};

#endif // STORAGE__RECORD_H_
