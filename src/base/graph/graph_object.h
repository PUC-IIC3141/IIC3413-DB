#ifndef BASE__OBJECT_TYPE_H_
#define BASE__OBJECT_TYPE_H_

#include <memory>
#include <string>

enum class ObjectType {
    node,
    edge,
    value_bool,
    value_float,
    value_int,
    value_string
};

class GraphObject {
public:
    virtual std::string to_string() const = 0;
    virtual ObjectType type() const = 0;

    virtual bool operator==(const GraphObject& rhs) const = 0;
    virtual bool operator!=(const GraphObject& rhs) const = 0;
    virtual bool operator<=(const GraphObject& rhs) const = 0;
    virtual bool operator>=(const GraphObject& rhs) const = 0;
    virtual bool operator<(const GraphObject& rhs) const = 0;
    virtual bool operator>(const GraphObject& rhs) const = 0;
};

#endif // BASE__OBJECT_TYPE_H_
