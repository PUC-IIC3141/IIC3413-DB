#ifndef RELATIONAL_MODEL__NODE_ENUM_H_
#define RELATIONAL_MODEL__NODE_ENUM_H_

#include "base/ids/graph_id.h"
#include "base/ids/object_id.h"
#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id_iter.h"

#include <memory>
#include <vector>

class NodeEnum : public BindingIdIter {
private:
    VarId var_id;
    uint64_t current_node = 0;
    uint64_t current_graph = 0;

    BindingId* my_input;
    std::unique_ptr<BindingId> my_binding;
    // vector of (mask, node_count), mask includes graph and node type
    std::vector<std::pair<uint64_t, uint64_t>> graph_counts;

public:
    NodeEnum(GraphId graph_id, VarId var_id);
    ~NodeEnum();

    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();
};

#endif // RELATIONAL_MODEL__NODE_ENUM_H_
