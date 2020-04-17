#ifndef RELATIONAL_MODEL__NODE_ENUM_H_
#define RELATIONAL_MODEL__NODE_ENUM_H_

#include "base/ids/graph_id.h"
#include "base/ids/object_id.h"
#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id_iter.h"

#include <memory>

class NodeEnum : public BindingIdIter {
private:
    GraphId graph_id;
    VarId var_id;
    uint64_t node_count;
    uint64_t current_node;

    BindingId* my_input;
    std::unique_ptr<BindingId> my_binding;

public:
    NodeEnum(GraphId graph_id, VarId var_id, uint64_t node_count);
    ~NodeEnum();

    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();
};

#endif // RELATIONAL_MODEL__NODE_ENUM_H_
