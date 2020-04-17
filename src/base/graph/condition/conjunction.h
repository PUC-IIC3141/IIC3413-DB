#ifndef BASE__COJUNCTION_H_
#define BASE__COJUNCTION_H_

#include "base/graph/condition/condition.h"

#include <memory>
#include <vector>
#include <iostream>

class Conjunction : public Condition {
public:
    std::vector<std::unique_ptr<Condition>> conditions;

    Conjunction() { }
    Conjunction(std::vector<std::unique_ptr<Condition>> conditions)
        : conditions (std::move(conditions)) { }

    bool eval(Binding& binding) {
        for (auto& condition : conditions) {
            if (!condition->eval(binding)) {
                return false;
            }
        }
        return true;
    }

    ConditionType type() {
        return ConditionType::conjunction;
    }

    void add(std::unique_ptr<Condition> condition) {
        conditions.push_back(std::move(condition));
    }

    void check_names(std::map<std::string, ObjectType>& m) {
        for (auto& condition : conditions) {
            condition->check_names(m);
        }
    }
};

#endif //BASE__COJUNCTION_H_
