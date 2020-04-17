#include "node_enum.h"

#include "relational_model/binding/binding_id.h"
#include "relational_model/relational_model.h"

using namespace std;

NodeEnum::NodeEnum(GraphId graph_id, VarId var_id, uint64_t node_count)
    : graph_id(graph_id), var_id(var_id), node_count(node_count) { }


NodeEnum::~NodeEnum() = default;

void NodeEnum::begin(BindingId& input) {
    my_binding = make_unique<BindingId>(input.var_count());
    my_input = &input;
    current_node = 0;
}


BindingId* NodeEnum::next() {
    current_node++;
    if (current_node <= node_count) {
        my_binding->add_all(*my_input);
        my_binding->add(var_id, current_node
                                | RelationalModel::NODE_MASK
                                | (graph_id << RelationalModel::GRAPH_OFFSET) );
        return my_binding.get();
    }
    else return nullptr;
}


void NodeEnum::reset(BindingId& input) {
    begin(input);
}
