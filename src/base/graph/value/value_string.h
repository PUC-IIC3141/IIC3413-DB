#ifndef BASE__VALUE_STRING_H_
#define BASE__VALUE_STRING_H_

#include "base/graph/value/value.h"

class ValueString : public Value {
public:
    const std::string value;

    ValueString(std::string value)
        : value(std::move(value)) { }
    ~ValueString() = default;

    // std::unique_ptr<std::vector<unsigned char>> get_bytes() const override {
    //     auto string_len = value.length();
    //     auto res = std::make_unique<std::vector<unsigned char>>(string_len);
	//     std::copy(value.begin(), value.end(), res->begin());
    //     return res;
    // }

    std::string to_string() const override {
        return '"' + value + '"';
    }

    ObjectType type() const override {
        return ObjectType::value_string;
    }

    bool operator==(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_string) {
            const auto& casted_rhs = static_cast<const ValueString&>(rhs);
            return this->value == casted_rhs.value;
        }
        return false;
    }

    bool operator!=(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_string) {
            const auto& casted_rhs = static_cast<const ValueString&>(rhs);
            return this->value != casted_rhs.value;
        }
        return true;
    }

    bool operator<=(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_string) {
            const auto& casted_rhs = static_cast<const ValueString&>(rhs);
            return this->value <= casted_rhs.value;
        }
        return false;
    }

    bool operator>=(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_string) {
            const auto& casted_rhs = static_cast<const ValueString&>(rhs);
            return this->value >= casted_rhs.value;
        }
        return false;
    }

    bool operator<(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_string) {
            const auto& casted_rhs = static_cast<const ValueString&>(rhs);
            return this->value < casted_rhs.value;
        }
        return false;
    }

    bool operator>(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::value_string) {
            const auto& casted_rhs = static_cast<const ValueString&>(rhs);
            return this->value > casted_rhs.value;
        }
        return false;
    }
};

#endif // BASE__VALUE_STRING_H_
