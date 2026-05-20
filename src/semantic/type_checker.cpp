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
    TypeInfo t1_check = t1;
    TypeInfo t2_check = t2;
    
    // Unwrap ARRAY types to get element type for checking
    if (t1.baseType == TypeCode::ARRAY && t1.ref > 0 && t1.ref < (int)st.atab.size()) {
        const AtabEntry& atabEntry = st.atab[t1.ref];
        t1_check = TypeInfo(static_cast<TypeCode>(atabEntry.etyp), atabEntry.eref);
    }
    
    if (t2.baseType == TypeCode::ARRAY && t2.ref > 0 && t2.ref < (int)st.atab.size()) {
        const AtabEntry& atabEntry = st.atab[t2.ref];
        t2_check = TypeInfo(static_cast<TypeCode>(atabEntry.etyp), atabEntry.eref);
    }
    
    // Unwrap RECORD types (keep ref for field checking if needed)
    if (t1.baseType == TypeCode::RECORD && t1.ref > 0) {
        // RECORD ref points to btab, which represents the record structure
        // Keep as is for now
        t1_check = t1;
    }
    
    if (t2.baseType == TypeCode::RECORD && t2.ref > 0) {
        t2_check = t2;
    }
    
    // Check REAL compatibility with INTEGER/SUBRANGE
    if (t1_check.baseType == TypeCode::REAL &&
       (t2_check.baseType == TypeCode::INTEGER || t2_check.baseType == TypeCode::SUBRANGE)) {
        return true;
    }

    if (isCompatible(t1_check, t2_check, st)) {
        if (t1_check.baseType == TypeCode::SUBRANGE) {
            if (t2_check.baseType == TypeCode::INTEGER || t2_check.baseType == TypeCode::SUBRANGE) {
                return true;
            }
        }
        if (t1_check.baseType == TypeCode::STRING && t2_check.baseType == TypeCode::STRING) {
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
    
    // Unwrap ARRAY types to get element type for checking
    TypeInfo left_check = left;
    TypeInfo right_check = right;
    
    if (left.baseType == TypeCode::ARRAY && left.ref > 0 && left.ref < (int)st.atab.size()) {
        const AtabEntry& atabEntry = st.atab[left.ref];
        left_check = TypeInfo(static_cast<TypeCode>(atabEntry.etyp), atabEntry.eref);
    }
    
    if (right.baseType == TypeCode::ARRAY && right.ref > 0 && right.ref < (int)st.atab.size()) {
        const AtabEntry& atabEntry = st.atab[right.ref];
        right_check = TypeInfo(static_cast<TypeCode>(atabEntry.etyp), atabEntry.eref);
    }

    if (op == "plus" || op == "minus" || op == "times") {
        if (!left_check.isNumeric() || !right_check.isNumeric()) return TypeInfo(TypeCode::NOTYPE);
        if (left_check.isReal() || right_check.isReal()) return TypeInfo(TypeCode::REAL);
        return TypeInfo(TypeCode::INTEGER);
    }

    if (op == "rdiv") {
        if (!left_check.isNumeric() || !right_check.isNumeric()) return TypeInfo(TypeCode::NOTYPE);
        return TypeInfo(TypeCode::REAL);
    }

    if (op == "idiv" || op == "imod") {
        if ((left_check.baseType == TypeCode::INTEGER || left_check.baseType == TypeCode::SUBRANGE) &&
            (right_check.baseType == TypeCode::INTEGER || right_check.baseType == TypeCode::SUBRANGE)) {
            return TypeInfo(TypeCode::INTEGER);
        }
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (op == "eql" || op == "neq" ||
        op == "lss" || op == "leq" ||
        op == "gtr" || op == "geq") {
        if (isCompatible(left_check, right_check, st)) {
            return TypeInfo(TypeCode::BOOLEAN);
        }
        if ((left_check.isReal() && right_check.isInteger()) ||
            (left_check.isInteger() && right_check.isReal())) {
            return TypeInfo(TypeCode::BOOLEAN);
        }
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (op == "andsy" || op == "orsy") {
        if (left_check.baseType == TypeCode::BOOLEAN &&
            right_check.baseType == TypeCode::BOOLEAN) {
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