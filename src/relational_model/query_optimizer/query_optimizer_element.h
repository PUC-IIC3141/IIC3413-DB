#ifndef RELATIONAL_MODEL__QUERY_OPTIMIZER_ELEMENT_H_
#define RELATIONAL_MODEL__QUERY_OPTIMIZER_ELEMENT_H_

#include "relational_model/physical_plan/binding_id_iter/graph_scan.h"

#include <string>
#include <memory>
#include <vector>

class QueryOptimizerElement {
public:
    virtual ~QueryOptimizerElement() { };
    virtual int get_heuristic() = 0;
    virtual void try_assign_var(VarId var_id) = 0;
    virtual std::vector<VarId> assign() = 0;
    virtual std::unique_ptr<BindingIdIter> get_scan() = 0;
};

#endif //RELATIONAL_MODEL__QUERY_OPTIMIZER_ELEMENT_H_
