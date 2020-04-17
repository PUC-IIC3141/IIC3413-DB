#ifndef BASE__CONDITION_H_
#define BASE__CONDITION_H_

class Binding;

#include <map>

#include "base/graph/graph_object.h"

enum class ConditionType {
    comparison,
    conjunction,
    constant,
    disjunction,
    negation
};

// Abstract class
class Condition {
public:
    virtual ~Condition() { };
    virtual bool eval(Binding&) = 0;
    virtual ConditionType type() = 0;
    virtual void check_names(std::map<std::string, ObjectType>&) = 0;
};

#endif //BASE__CONDITION_H_
