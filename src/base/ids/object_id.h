#ifndef BASE__OBJECT_ID_H_
#define BASE__OBJECT_ID_H_

#include <memory>

class ObjectId {
public:
    static constexpr uint64_t NULL_OBJECT_ID = 0;
    static constexpr uint64_t OBJECT_ID_NOT_FOUND = UINT64_MAX;

    uint64_t id;

    ObjectId(uint64_t id);
    ObjectId(); // initialized with NULL_OBJECT_ID
    ~ObjectId();

    ObjectId(const ObjectId&); // copy constructor
    static ObjectId get_null();
    bool is_null();
    bool not_found();

    operator uint64_t() const;

    void operator = (const ObjectId& other);
    bool operator ==(const ObjectId& rhs) const;
    bool operator !=(const ObjectId& rhs) const;
};

#endif // BASE__OBJECT_ID_H_
