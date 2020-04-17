#ifndef BASE__VALUE_ASSIGN_VARIABLE_H_
#define BASE__VALUE_ASSIGN_VARIABLE_H_

#include "base/graph/condition/value_assign.h"
#include "base/parser/logical_plan/exceptions.h"

class ValueAssignVariable : public ValueAssign {
private:
    std::string var;
    std::string key;
public:
    ValueAssignVariable(std::string var, std::string key)
        : var(std::move(var)), key(std::move(key)) { }
    ~ValueAssignVariable() = default;

    std::shared_ptr<GraphObject> get_value(Binding& binding) {
        return binding.get(var, key);
    }

    void check_names(std::map<std::string, ObjectType>& m) {
        auto search = m.find(var);
        if (search == m.end()) {
            throw QuerySemanticException("Variable \"" + var + "\" used in WHERE is not declared in MATCH");
        }
    }
};

#endif //BASE__VALUE_ASSIGN_VARIABLE_H_
