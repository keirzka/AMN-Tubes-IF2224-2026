#include "runtime_value.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

RuntimeValue RuntimeValue::fromInt(int v) {
    RuntimeValue rv;
    rv.type = ValueType::INTEGER;
    rv.ival = v;
    return rv;
}

RuntimeValue RuntimeValue::fromReal(double v) {
    RuntimeValue rv;
    rv.type = ValueType::REAL;
    rv.dval = v;
    return rv;
}

RuntimeValue RuntimeValue::fromBool(bool v) {
    RuntimeValue rv;
    rv.type = ValueType::BOOLEAN;
    rv.bval = v;
    return rv;
}

RuntimeValue RuntimeValue::fromChar(char v) {
    RuntimeValue rv;
    rv.type = ValueType::CHAR;
    rv.cval = v;
    return rv;
}

RuntimeValue RuntimeValue::fromString(const std::string& v) {
    RuntimeValue rv;
    rv.type = ValueType::STRING;
    rv.sval = v;
    return rv;
}

std::string RuntimeValue::toString() const {
    switch (type) {
        case ValueType::INTEGER:
            return std::to_string(ival);

        case ValueType::REAL: {
            std::ostringstream oss;
            oss << dval;
            std::string s = oss.str();
            if (s.find('.') == std::string::npos &&
                s.find('e') == std::string::npos &&
                s.find('E') == std::string::npos) {
                s += ".0";
            }
            return s;
        }

        case ValueType::BOOLEAN:
            return bval ? "TRUE" : "FALSE";

        case ValueType::CHAR:
            return std::string(1, cval);

        case ValueType::STRING:
            return sval;

        case ValueType::NIL:
            return "(nil)";

        default:
            return "(unknown)";
    }
}

bool RuntimeValue::toBool() const {
    switch (type) {
        case ValueType::BOOLEAN: return bval;
        case ValueType::INTEGER: return ival != 0;
        case ValueType::NIL:     return false;
        default:                 return true;
    }
}

int RuntimeValue::toInt() const {
    switch (type) {
        case ValueType::INTEGER: return ival;
        case ValueType::BOOLEAN: return bval ? 1 : 0;
        case ValueType::CHAR:    return static_cast<int>(static_cast<unsigned char>(cval));
        case ValueType::REAL:    return static_cast<int>(dval);
        default:
            throw std::runtime_error(
                "RuntimeValue::toInt() dipanggil pada tipe " + typeName());
    }
}

double RuntimeValue::toReal() const {
    switch (type) {
        case ValueType::REAL:    return dval;
        case ValueType::INTEGER: return static_cast<double>(ival);
        case ValueType::CHAR:    return static_cast<double>(static_cast<unsigned char>(cval));
        default:
            throw std::runtime_error(
                "RuntimeValue::toReal() dipanggil pada tipe " + typeName());
    }
}

bool RuntimeValue::operator==(const RuntimeValue& other) const {
    if (type == ValueType::INTEGER && other.type == ValueType::INTEGER)
        return ival == other.ival;
    if (type == ValueType::REAL && other.type == ValueType::REAL)
        return dval == other.dval;
    if ((type == ValueType::INTEGER && other.type == ValueType::REAL) ||
        (type == ValueType::REAL    && other.type == ValueType::INTEGER))
        return toReal() == other.toReal();
    if (type == ValueType::BOOLEAN && other.type == ValueType::BOOLEAN)
        return bval == other.bval;
    if (type == ValueType::CHAR && other.type == ValueType::CHAR)
        return cval == other.cval;
    if (type == ValueType::STRING && other.type == ValueType::STRING)
        return sval == other.sval;
    if (type == ValueType::NIL && other.type == ValueType::NIL)
        return true;
    return false;
}

bool RuntimeValue::operator!=(const RuntimeValue& other) const {
    return !(*this == other);
}

bool RuntimeValue::operator<(const RuntimeValue& other) const {
    if (type == ValueType::INTEGER && other.type == ValueType::INTEGER)
        return ival < other.ival;
    if (type == ValueType::REAL && other.type == ValueType::REAL)
        return dval < other.dval;
    if ((type == ValueType::INTEGER && other.type == ValueType::REAL) ||
        (type == ValueType::REAL    && other.type == ValueType::INTEGER))
        return toReal() < other.toReal();
    if (type == ValueType::CHAR && other.type == ValueType::CHAR)
        return cval < other.cval;
    if (type == ValueType::STRING && other.type == ValueType::STRING)
        return sval < other.sval;
    return false;
}

bool RuntimeValue::operator<=(const RuntimeValue& other) const {
    return *this < other || *this == other;
}

bool RuntimeValue::operator>(const RuntimeValue& other) const {
    return other < *this;
}

bool RuntimeValue::operator>=(const RuntimeValue& other) const {
    return !(*this < other);
}

std::string RuntimeValue::typeName() const {
    return valueTypeToString(type);
}

std::string valueTypeToString(ValueType vt) {
    switch (vt) {
        case ValueType::NIL:     return "NIL";
        case ValueType::INTEGER: return "INTEGER";
        case ValueType::REAL:    return "REAL";
        case ValueType::BOOLEAN: return "BOOLEAN";
        case ValueType::CHAR:    return "CHAR";
        case ValueType::STRING:  return "STRING";
        default:                 return "UNKNOWN";
    }
}