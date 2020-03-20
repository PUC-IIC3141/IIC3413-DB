#ifndef BASE__VALUE_INT_H_
#define BASE__VALUE_INT_H_

#include "base/graph/value/value.h"

#include <cstring>

class ValueInt: public Value {
public:
    const int32_t value;

    ValueInt(int32_t value);
    ~ValueInt();
    std::unique_ptr<std::vector<unsigned char>> get_bytes() const override;

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
