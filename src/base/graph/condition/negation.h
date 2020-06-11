#ifndef BASE__NEGATION_H_
#define BASE__NEGATION_H_

#include <memory>

#include "base/graph/condition/condition.h"

class Negation : public Condition {
public:
    std::unique_ptr<Condition> condition;

    Negation(std::unique_ptr<Condition> condition)
        : condition(std::move(condition)) { }

    bool eval(Binding& binding) {
        return !condition->eval(binding);
    }

    ConditionType type() {
        return ConditionType::negation;
    }

    void check_names(std::map<std::string, ObjectType>& m) {
        condition->check_names(m);
    }
};

#endif // BASE__NEGATION_H_
