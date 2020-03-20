#ifndef BASE__VALUE_ASSIGN_H_
#define BASE__VALUE_ASSIGN_H_

#include <memory>

class Binding;
class Value;

// Abstract class
class ValueAssign {
public:
    virtual std::shared_ptr<GraphObject> get_value(Binding&) = 0;
};

#endif //BASE__VALUE_ASSIGN_H_
