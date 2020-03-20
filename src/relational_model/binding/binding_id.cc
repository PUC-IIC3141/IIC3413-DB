#include "binding_id.h"

using namespace std;

BindingId::BindingId(int_fast32_t var_count) {
    dict = vector<ObjectId>(var_count);
}


int_fast32_t BindingId::var_count() {
    return dict.size();
}


void BindingId::add_all(BindingId& other) {
    for (size_t i = 0; i < other.dict.size(); ++i) {
        if (!other.dict[i].is_null()) {
            this->dict[i] = other.dict[i];
        }
    }
}


void BindingId::add(VarId var_id, ObjectId id) {
    dict[var_id] = id;
}


ObjectId BindingId::operator[](VarId var_id) {
    return dict[var_id];
}


void BindingId::print(vector<string>& var_names) const {
    std::cout << "BindingId: ";
    for (size_t i = 0; i < dict.size(); ++i) {
        std::cout << var_names[i] << ":[" << (dict[i] >> 56) << "]" << (dict[i] & 0x00FFFFFFFFFFFFFFUL) << "\t";
    }
    std::cout << endl;
}
