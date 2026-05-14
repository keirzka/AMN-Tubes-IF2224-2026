#include "type_checker.hpp"
#include <sstream>
#include <algorithm>

TypeInfo::TypeInfo()
    : baseType(TypeCode::NOTYPE), ref(0), low(0), high(0),
      subrangeBaseType(TypeCode::NOTYPE),
      isNamed(false), namedId("") {}

TypeInfo::TypeInfo(TypeCode tc, int r, int lo, int hi,
                   bool named, const std::string& nid)
    : baseType(tc), ref(r), low(lo), high(hi),
      subrangeBaseType(TypeCode::NOTYPE),
      isNamed(named), namedId(nid) {}

bool TypeInfo::isNumeric() const {
    return baseType == TypeCode::INTEGER || baseType == TypeCode::REAL ||
           baseType == TypeCode::SUBRANGE;
}

bool TypeInfo::isOrdinal() const {
    return baseType == TypeCode::INTEGER  ||
           baseType == TypeCode::CHAR     ||
           baseType == TypeCode::BOOLEAN  ||
           baseType == TypeCode::SUBRANGE ||
           baseType == TypeCode::ENUMERATED;
}

bool TypeInfo::isSimple() const {
    return baseType != TypeCode::ARRAY && baseType != TypeCode::RECORD;
}

bool TypeInfo::isReal()       const { return baseType == TypeCode::REAL; }
bool TypeInfo::isInteger()    const { return baseType == TypeCode::INTEGER || baseType == TypeCode::SUBRANGE; }
bool TypeInfo::isBoolean()    const { return baseType == TypeCode::BOOLEAN; }
bool TypeInfo::isChar()       const { return baseType == TypeCode::CHAR; }
bool TypeInfo::isString()     const { return baseType == TypeCode::STRING; }
bool TypeInfo::isArray()      const { return baseType == TypeCode::ARRAY; }
bool TypeInfo::isRecord()     const { return baseType == TypeCode::RECORD; }
bool TypeInfo::isSubrange()   const { return baseType == TypeCode::SUBRANGE; }
bool TypeInfo::isEnumerated() const { return baseType == TypeCode::ENUMERATED; }

std::string TypeInfo::toString() const {
    switch (baseType) {
        case TypeCode::NOTYPE:     return "notype";
        case TypeCode::INTEGER:    return "integer";
        case TypeCode::REAL:       return "real";
        case TypeCode::CHAR:       return "char";
        case TypeCode::BOOLEAN:    return "boolean";
        case TypeCode::STRING:     return "string";
        case TypeCode::ARRAY:      return "array";
        case TypeCode::RECORD:
            if (isNamed) return "record(" + namedId + ")";
            return "record(anonymous)";
        case TypeCode::SUBRANGE: {
            std::ostringstream oss;
            oss << "subrange[" << low << ".." << high << "]";
            return oss.str();
        }
        case TypeCode::ENUMERATED:
            if (isNamed) return "enumerated(" + namedId + ")";
            return "enumerated";
        default: return "unknown";
    }
}

static TypeCode effectiveBaseType(const TypeInfo& t) {
    if (t.baseType == TypeCode::SUBRANGE) {
        if (t.subrangeBaseType != TypeCode::NOTYPE) return t.subrangeBaseType;
        return TypeCode::INTEGER;
    }
    return t.baseType;
}

bool isCompatible(const TypeInfo& t1, const TypeInfo& t2, const SymbolTable& st) {
    if (t1.baseType == t2.baseType) {
        if (t1.baseType == TypeCode::RECORD) {
            if (!t1.isNamed || !t2.isNamed) return false;
            return t1.namedId == t2.namedId;
        }
        if (t1.baseType == TypeCode::ENUMERATED) {
            if (!t1.isNamed || !t2.isNamed) return false;
            return t1.namedId == t2.namedId;
        }
        if (t1.baseType == TypeCode::ARRAY) {
            return t1.ref == t2.ref;
        }
        if (t1.baseType == TypeCode::SUBRANGE && t2.baseType == TypeCode::SUBRANGE) {
            return effectiveBaseType(t1) == effectiveBaseType(t2);
        }
        return true;
    }

    bool t1Sub = (t1.baseType == TypeCode::SUBRANGE);
    bool t2Sub = (t2.baseType == TypeCode::SUBRANGE);

    if (t1Sub || t2Sub) {
        TypeCode base1 = effectiveBaseType(t1);
        TypeCode base2 = effectiveBaseType(t2);
        return base1 == base2;
    }

    return false;
}

bool isAssignmentCompatible(const TypeInfo& t1, const TypeInfo& t2, const SymbolTable& st) {
    if (t1.baseType == TypeCode::REAL &&
       (t2.baseType == TypeCode::INTEGER || t2.baseType == TypeCode::SUBRANGE)) {
        return true;
    }

    if (isCompatible(t1, t2, st)) {
        if (t1.baseType == TypeCode::SUBRANGE) {
            if (t2.baseType == TypeCode::INTEGER || t2.baseType == TypeCode::SUBRANGE) {
                return true;
            }
        }
        if (t1.baseType == TypeCode::STRING && t2.baseType == TypeCode::STRING) {
            return true;
        }
        return true;
    }

    return false;
}

TypeInfo resultType(const std::string& op,
                    const TypeInfo& left,
                    const TypeInfo& right,
                    const SymbolTable& st) {

    if (op == "plus" || op == "minus" || op == "times") {
        if (!left.isNumeric() || !right.isNumeric()) return TypeInfo(TypeCode::NOTYPE);
        if (left.isReal() || right.isReal()) return TypeInfo(TypeCode::REAL);
        return TypeInfo(TypeCode::INTEGER);
    }

    if (op == "rdiv") {
        if (!left.isNumeric() || !right.isNumeric()) return TypeInfo(TypeCode::NOTYPE);
        return TypeInfo(TypeCode::REAL);
    }

    if (op == "idiv" || op == "imod") {
        if ((left.baseType == TypeCode::INTEGER || left.baseType == TypeCode::SUBRANGE) &&
            (right.baseType == TypeCode::INTEGER || right.baseType == TypeCode::SUBRANGE)) {
            return TypeInfo(TypeCode::INTEGER);
        }
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (op == "eql" || op == "neq" ||
        op == "lss" || op == "leq" ||
        op == "gtr" || op == "geq") {
        if (isCompatible(left, right, st)) {
            return TypeInfo(TypeCode::BOOLEAN);
        }
        if ((left.isReal() && right.isInteger()) ||
            (left.isInteger() && right.isReal())) {
            return TypeInfo(TypeCode::BOOLEAN);
        }
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (op == "andsy" || op == "orsy") {
        if (left.baseType == TypeCode::BOOLEAN &&
            right.baseType == TypeCode::BOOLEAN) {
            return TypeInfo(TypeCode::BOOLEAN);
        }
        return TypeInfo(TypeCode::NOTYPE);
    }

    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo resultTypeUnary(const std::string& op, const TypeInfo& operand) {
    if (op == "notsy") {
        if (operand.baseType == TypeCode::BOOLEAN) return TypeInfo(TypeCode::BOOLEAN);
        return TypeInfo(TypeCode::NOTYPE);
    }
    if (op == "plus" || op == "minus") {
        if (operand.isReal())    return TypeInfo(TypeCode::REAL);
        if (operand.isInteger()) return TypeInfo(TypeCode::INTEGER);
        return TypeInfo(TypeCode::NOTYPE);
    }
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo typeInfoFromTab(int tabIdx, const SymbolTable& st) {
    if (tabIdx < 0 || tabIdx >= (int)st.tab.size()) {
        return TypeInfo(TypeCode::NOTYPE);
    }
    const TabEntry& e = st.tab[tabIdx];
    TypeInfo ti;
    ti.baseType = e.type;
    ti.ref      = e.ref;

    if (e.type == TypeCode::ARRAY && e.ref > 0 && e.ref < (int)st.atab.size()) {
        ti.low  = st.atab[e.ref].low;
        ti.high = st.atab[e.ref].high;
    }
    if (e.type == TypeCode::SUBRANGE && e.ref > 0 && e.ref < (int)st.atab.size()) {
        ti.low  = st.atab[e.ref].low;
        ti.high = st.atab[e.ref].high;
        ti.subrangeBaseType = static_cast<TypeCode>(st.atab[e.ref].etyp);
    }
    if (e.type == TypeCode::RECORD) {
        ti.isNamed  = true;
        ti.namedId  = e.identifier;
    }
    return ti;
}

bool isValidIndexType(const TypeInfo& t) {
    if (!t.isSimple())                    return false;
    if (t.baseType == TypeCode::REAL)     return false;
    if (t.baseType == TypeCode::STRING)   return false;
    if (t.baseType == TypeCode::NOTYPE)   return false;
    return true;
}

bool isValidSubrangeType(const TypeInfo& t) {
    if (t.baseType == TypeCode::REAL)     return false;
    if (t.baseType == TypeCode::NOTYPE)   return false;
    if (t.baseType == TypeCode::ARRAY)    return false;
    if (t.baseType == TypeCode::RECORD)   return false;
    return true;
}

bool isBooleanType(const TypeInfo& t) {
    return t.baseType == TypeCode::BOOLEAN;
}

bool isOrdinalType(const TypeInfo& t) {
    return t.isOrdinal();
}