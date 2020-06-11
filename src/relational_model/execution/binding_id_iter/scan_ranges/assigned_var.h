#ifndef RELATIONAL_MODEL__ASSIGNED_VAR_H_
#define RELATIONAL_MODEL__ASSIGNED_VAR_H_

#include "relational_model/execution/binding_id_iter/scan_ranges/scan_range.h"

class AssignedVar : public ScanRange {
private:
    VarId var_id;

public:
    AssignedVar(VarId var_id)
        : var_id(var_id) { }

    uint64_t get_min(BindingId& binding_id) override {
        auto obj_id = binding_id[var_id];
        if (obj_id.is_null()) {
            throw std::logic_error("var should be assigned in binding");
        } else {
            return obj_id;
        }
    }

    uint64_t get_max(BindingId& binding_id) override {
        auto obj_id = binding_id[var_id];
        if (obj_id.is_null()) {
            throw std::logic_error("var should be assigned in binding");
        } else {
            return obj_id;
        }
    }

    void try_assign(BindingId&, ObjectId) override { }
};

#endif // RELATIONAL_MODEL__ASSIGNED_VAR_H_
