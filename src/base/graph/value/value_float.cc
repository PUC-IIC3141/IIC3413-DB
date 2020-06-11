#include "value_float.h"
#include "value_int.h"

using namespace std;

ValueFloat::ValueFloat(float value)
    : value(value) { }


ValueFloat::~ValueFloat() = default;


string ValueFloat::to_string() const {
    return std::to_string(value);
}


ObjectType ValueFloat::type() const {
    return ObjectType::value_float;
}


bool ValueFloat::operator==(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return this->value == casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value == casted_rhs.value;
    }
    else return false;
}


bool ValueFloat::operator!=(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return this->value != casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value != casted_rhs.value;
    }
    else return true;
}


bool ValueFloat::operator<=(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return this->value <= casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value <= casted_rhs.value;
    }
    else return false;
}


bool ValueFloat::operator>=(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return this->value == casted_rhs.value;
    }
    else if (rhs.type() >= ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value >= casted_rhs.value;
    }
    else return false;
}


bool ValueFloat::operator<(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return this->value < casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value < casted_rhs.value;
    }
    else return false;
}


bool ValueFloat::operator>(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return this->value > casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value > casted_rhs.value;
    }
    else return false;
}
