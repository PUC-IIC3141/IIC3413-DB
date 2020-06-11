#ifndef BASE__VALUE_ASSIGN_H_
#define BASE__VALUE_ASSIGN_H_

#include <memory>

class Binding;

// Abstract class
class ValueAssign {
public:
    virtual ~ValueAssign() { };
    virtual std::shared_ptr<GraphObject> get_value(Binding&) = 0;
    virtual void check_names(std::map<std::string, ObjectType>& m) = 0;
};

#endif // BASE__VALUE_ASSIGN_H_
