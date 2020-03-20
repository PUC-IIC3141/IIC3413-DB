#ifndef BASE__VALUE_FLOAT_H_
#define BASE__VALUE_FLOAT_H_

#include "base/graph/value/value.h"

#include <cstring>

class ValueFloat: public Value {
public:
    const float value;

    ValueFloat(float value);
    ~ValueFloat();

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


#endif // BASE__VALUE_FLOAT_H_
