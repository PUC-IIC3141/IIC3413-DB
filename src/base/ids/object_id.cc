#include "object_id.h"

ObjectId::ObjectId(uint64_t id)
    : id(id) { }


ObjectId::ObjectId()
    : id(NULL_OBJECT_ID) { }


ObjectId::~ObjectId() = default;


ObjectId::ObjectId(const ObjectId& obj)
    : id(obj.id) { }


ObjectId ObjectId::get_null() {
    return ObjectId(NULL_OBJECT_ID);
}

bool ObjectId::is_null() {
    return id == NULL_OBJECT_ID;
}


bool ObjectId::not_found() {
    return id == OBJECT_ID_NOT_FOUND;
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
