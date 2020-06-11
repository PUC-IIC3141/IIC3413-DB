#include "node_enum.h"

#include "relational_model/relational_model.h"
#include "storage/catalog/catalog.h"

using namespace std;

NodeEnum::NodeEnum(GraphId graph_id, VarId var_id)
    : var_id(var_id)
{
    if (graph_id.is_default()) {
        auto graph_count = catalog.get_graph_count();
        for (uint64_t i = 1; i <= graph_count; i++) {
            graph_counts.push_back(make_pair(
                (i << RelationalModel::GRAPH_OFFSET) | RelationalModel::NODE_MASK,
                catalog.get_node_count(GraphId(i))
            ));
        }
    } else {
        graph_counts.push_back(make_pair(
            graph_id << RelationalModel::GRAPH_OFFSET,
            catalog.get_node_count(graph_id)
        ));
    }
}


NodeEnum::~NodeEnum() = default;

void NodeEnum::begin(BindingId& input) {
    my_binding = make_unique<BindingId>(input.var_count());
    my_input = &input;
    current_node = 0;
}


BindingId* NodeEnum::next() {
    while (current_graph < graph_counts.size()) {
        ++current_node;
        if (current_node <= graph_counts[current_graph].second) {
            my_binding->add_all(*my_input);
            my_binding->add(var_id, current_node | graph_counts[current_graph].first );
            return my_binding.get();
        }
        else {
            ++current_graph;
            current_node = 0;
            // while executes again
        }
    }
    return nullptr;
}


void NodeEnum::reset(BindingId& input) {
    begin(input);
}
