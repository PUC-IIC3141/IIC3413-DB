#include "value_int.h"
#include "value_float.h"

using namespace std;

ValueInt::ValueInt(int64_t value)
    : value(value) { }


ValueInt::~ValueInt() = default;


string ValueInt::to_string() const {
    return std::to_string(value);
}


ObjectType ValueInt::type() const {
    return ObjectType::value_int;
}


bool ValueInt::operator==(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value == casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return casted_rhs.value == this->value; // Left defines precision, so float needs to be at left
    }
    else return false;
}


bool ValueInt::operator!=(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value != casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return casted_rhs.value != this->value; // Left defines precision, so float needs to be at left
    }
    else return true;
}


bool ValueInt::operator<=(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value <= casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return casted_rhs.value >= this->value; // Left defines precision, so float needs to be at left
    }
    else return false;
}


bool ValueInt::operator>=(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value >= casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return casted_rhs.value <= this->value; // Left defines precision, so float needs to be at left
    }
    else return false;
}


bool ValueInt::operator<(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value < casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return casted_rhs.value > this->value; // Left defines precision, so float needs to be at left
    }
    else return false;
}


bool ValueInt::operator>(const GraphObject& rhs) const {
    if (rhs.type() == ObjectType::value_int) {
        const auto& casted_rhs = static_cast<const ValueInt&>(rhs);
        return this->value > casted_rhs.value;
    }
    else if (rhs.type() == ObjectType::value_float) {
        const auto& casted_rhs = static_cast<const ValueFloat&>(rhs);
        return casted_rhs.value < this->value; // Left defines precision, so float needs to be at left
    }
    else return false;
}
