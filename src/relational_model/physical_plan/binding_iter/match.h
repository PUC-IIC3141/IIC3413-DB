#ifndef RELATIONAL_MODEL__MATCH_H_
#define RELATIONAL_MODEL__MATCH_H_

#include "base/binding/binding_iter.h"
#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/binding/binding_id_iter.h"
#include "relational_model/query_optimizer/query_optimizer_element.h"

#include <map>
#include <memory>
#include <vector>

class Match : public BindingIter {

private:
    std::vector<std::unique_ptr<QueryOptimizerElement>> elements;
    std::unique_ptr<BindingIdIter> root;
    const std::map<std::string, VarId> var_pos;
    std::unique_ptr<BindingId> binding_id;

    std::unique_ptr<BindingIdIter> get_join_plan();

public:
    Match(std::unique_ptr<BindingIdIter> root, std::map<std::string, VarId> var_pos);
    Match(std::vector<std::unique_ptr<QueryOptimizerElement>> elements, std::map<std::string, VarId> var_pos);
    ~Match() = default;

    void begin();
    std::unique_ptr<Binding> next();
};

#endif //RELATIONAL_MODEL__MATCH_H_
