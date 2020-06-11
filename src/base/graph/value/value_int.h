#ifndef BASE__VALUE_INT_H_
#define BASE__VALUE_INT_H_

#include <cstring>

#include "base/graph/value/value.h"

class ValueInt: public Value {
public:
    const int64_t value;

    ValueInt(int64_t value);
    ~ValueInt();

    std::string to_string() const override;

    ObjectType type() const override;

    bool operator==(const GraphObject& rhs) const override;
    bool operator!=(const GraphObject& rhs) const override;
    bool operator<=(const GraphObject& rhs) const override;
    bool operator>=(const GraphObject& rhs) const override;
    bool operator<(const GraphObject& rhs) const override;
    bool operator>(const GraphObject& rhs) const override;
};

#endif // BASE__VALUE_INT_H_
