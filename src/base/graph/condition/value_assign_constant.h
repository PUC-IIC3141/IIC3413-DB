#ifndef BASE__VALUE_ASSIGN_CONSTANT_H_
#define BASE__VALUE_ASSIGN_CONSTANT_H_

#include "base/graph/condition/value_assign.h"

class ValueAssignConstant : public ValueAssign {
private:
    std::shared_ptr<GraphObject> value;

public:
    ValueAssignConstant(std::unique_ptr<Value> value)
        : value(std::move(value)) { }
    ~ValueAssignConstant() = default;

    std::shared_ptr<GraphObject> get_value(Binding&) {
        return value;
    }

    void check_names(std::map<std::string, ObjectType>&) { }
};

#endif //BASE__VALUE_ASSIGN_CONSTANT_H_
