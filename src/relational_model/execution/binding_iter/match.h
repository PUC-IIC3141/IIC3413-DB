#ifndef RELATIONAL_MODEL__MATCH_H_
#define RELATIONAL_MODEL__MATCH_H_

#include <map>
#include <memory>
#include <vector>

#include "base/binding/binding_iter.h"
#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id_iter.h"
#include "relational_model/query_optimizer/join_plan/join_plan.h"

class Match : public BindingIter {

private:
    std::unique_ptr<BindingIdIter> root;
    const std::map<std::string, VarId> var_pos;
    std::unique_ptr<BindingId> binding_id;

public:
    Match(std::unique_ptr<BindingIdIter> root,
          std::map<std::string, VarId> var_pos);
    ~Match() = default;

    void begin();
    std::unique_ptr<Binding> next();
};

#endif // RELATIONAL_MODEL__MATCH_H_
