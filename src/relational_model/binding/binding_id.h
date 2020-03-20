#ifndef RELATIONAL_MODEL__BINDING_ID_H_
#define RELATIONAL_MODEL__BINDING_ID_H_

#include <iostream>
#include <iterator>
#include <vector>

#include "base/ids/var_id.h"
#include "base/ids/object_id.h"

class BindingId {

private:
    std::vector<ObjectId> dict;

public:
    BindingId(int_fast32_t var_count);
    ~BindingId() = default;

    ObjectId operator[](VarId);

    int_fast32_t var_count();
    void add_all(BindingId&);
    void add(VarId, ObjectId);

    void print(std::vector<std::string>& var_names) const;
};


#endif //RELATIONAL_MODEL__BINDING_ID_H_
