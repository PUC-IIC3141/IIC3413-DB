#ifndef BASE__CONDITION_H_
#define BASE__CONDITION_H_

class Binding;

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
    virtual bool eval(Binding&) = 0;
    virtual ConditionType type() = 0;
};

#endif //BASE__CONDITION_H_
