#ifndef BASE__OBJECT_ID_H_
#define BASE__OBJECT_ID_H_

#include <memory>

const uint64_t NULL_OBJECT_ID      = 0;
const uint64_t NOT_FOUND_OBJECT_ID = UINT64_MAX;

class ObjectFile;
class Value;

class ObjectId {

public:
    uint64_t id;

    ObjectId(uint64_t id);
    ObjectId(); // initialized with NULL_OBJECT_ID
    ~ObjectId();

    bool is_null();
    bool not_found();

    operator uint64_t() const;

    void operator = (const ObjectId& other);
    bool operator ==(const ObjectId& rhs) const;
    bool operator !=(const ObjectId& rhs) const;

};

#endif //BASE__OBJECT_ID_H_
