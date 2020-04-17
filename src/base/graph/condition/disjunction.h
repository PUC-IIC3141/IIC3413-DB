#ifndef BASE__DISJUNCTION_H_
#define BASE__DISJUNCTION_H_

#include "base/graph/condition/condition.h"

#include <memory>
#include <vector>
#include <iostream>

class Disjunction : public Condition {
public:
    std::vector<std::unique_ptr<Condition>> conditions;

    Disjunction() { }
    Disjunction(std::vector<std::unique_ptr<Condition>> conditions)
        : conditions (std::move(conditions)) { }

    bool eval(Binding& binding) {
        // std::cout << "eval disjunction. conditions: " << conditions.size() << "\n";
        for (auto& condition : conditions) {
            if (condition->eval(binding)) {
                return true;
            }
        }
        return false;
    }

    ConditionType type() {
        return ConditionType::disjunction;
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

#endif //BASE__DISJUNCTION_H_
