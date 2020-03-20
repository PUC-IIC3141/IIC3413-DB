#include "object_id.h"

ObjectId::ObjectId(uint64_t id)
    : id(id) { }


ObjectId::ObjectId()
    : id(NULL_OBJECT_ID) { }


ObjectId::~ObjectId() = default;


bool ObjectId::is_null() {
    return id == NULL_OBJECT_ID;
}


bool ObjectId::not_found() {
    return id == NOT_FOUND_OBJECT_ID;
}


ObjectId::operator uint64_t() const {
    return id;
}


void ObjectId::operator=(const ObjectId& other) {
    this->id = other.id;
}


bool ObjectId::operator==(const ObjectId& rhs) const {
    return id == rhs.id;
}


bool ObjectId::operator!=(const ObjectId& rhs) const {
    return id != rhs.id;
}
