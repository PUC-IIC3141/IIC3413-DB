#ifndef BASE__VALUE_BOOL_H_
#define BASE__VALUE_BOOL_H_

#include "base/graph/value/value.h"

class ValueBool: public Value {
public:
    const bool value;

    ValueBool(bool value)
        : value(value) { }
    ~ValueBool() = default;

    std::unique_ptr<std::vector<unsigned char>> get_bytes() const override {
        return std::make_unique<std::vector<unsigned char>>(1, (unsigned char)value);
    }

    std::string to_string() const override {
        if (value)
            return "true";
        else
            return "false";
    }

    ObjectType type() const override {
        return ObjectType::value_bool;
    }

    bool operator==(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_bool) {
            const auto& casted_rhs = static_cast<const ValueBool&>(rhs);
            return this->value == casted_rhs.value;
        }
        return false;
    }

    bool operator!=(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_bool) {
            const auto& casted_rhs = static_cast<const ValueBool&>(rhs);
            return this->value != casted_rhs.value;
        }
        return true;
    }

    bool operator<=(const GraphObject&) const override {
        return false;
    }

    bool operator>=(const GraphObject&) const override {
        return false;
    }

    bool operator<(const GraphObject&) const override {
        return false;
    }

    bool operator>(const GraphObject&) const override {
        return false;
    }
};


#endif // BASE__VALUE_BOOL_H_
