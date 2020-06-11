#ifndef RELATIONAL_MODEL__NAMED_GRAPH_VAR_H_
#define RELATIONAL_MODEL__NAMED_GRAPH_VAR_H_

#include "base/ids/graph_id.h"
#include "relational_model/relational_model.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/scan_range.h"

class NamedGraphVar : public ScanRange {
private:
    VarId var_id;
    uint64_t mask;

public:
    NamedGraphVar(VarId var_id, GraphId graph_id, ObjectType element_type)
        : var_id(var_id)
    {
        if (element_type == ObjectType::node) {
            mask = RelationalModel::NODE_MASK;
        } else if (element_type == ObjectType::edge) {
            mask = RelationalModel::EDGE_MASK;
        } else {
            throw std::logic_error("NamedGraphVar can receive only node or edge");
        }
        mask |= graph_id << RelationalModel::GRAPH_OFFSET;
    }

    uint64_t get_min(BindingId&) override {
        return mask;
    }

    uint64_t get_max(BindingId&) override {
        return mask | 0x00'0000'FFFFFFFFFFUL;
    }

    void try_assign(BindingId& binding, ObjectId obj_id) override {
        binding.add(var_id, obj_id);
    }
};

#endif // RELATIONAL_MODEL__NAMED_GRAPH_VAR_H_
