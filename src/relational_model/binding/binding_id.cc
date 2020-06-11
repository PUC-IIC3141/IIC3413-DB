#include "binding_id.h"

using namespace std;

BindingId::BindingId(int_fast32_t var_count) {
    object_ids = vector<ObjectId>(var_count);
    static_assert(ObjectId::NULL_OBJECT_ID == 0,
        "If NULL_OBJECT_ID != 0, need to explicitally set values to NULL_OBJECT_ID");
}


int_fast32_t BindingId::var_count() {
    return object_ids.size();
}


void BindingId::add_all(BindingId& other) {
    for (size_t i = 0; i < other.object_ids.size(); ++i) {
        if (!other.object_ids[i].is_null()) {
            this->object_ids[i] = other.object_ids[i];
        }
    }
}


void BindingId::add(VarId var_id, ObjectId id) {
    object_ids[var_id] = id;
}


ObjectId BindingId::operator[](VarId var_id) {
    return object_ids[var_id];
}


void BindingId::print() const {
    std::cout << "BindingId: ";
    for (size_t i = 0; i < object_ids.size(); ++i) {
        std::cout << i << ":[" << (object_ids[i] >> 56) << "]" << (object_ids[i] & 0x00'0000'FFFFFFFFFFUL) << "\t";
    }
    std::cout << endl;
}
