#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include "symbol_table.hpp"
#include <string>

struct TypeInfo {
    TypeCode baseType;
    int ref;
    int low;
    int high;
    TypeCode subrangeBaseType;
    bool isNamed;
    std::string namedId;

    TypeInfo();
    TypeInfo(TypeCode tc, int r = 0, int lo = 0, int hi = 0,
             bool named = false, const std::string& nid = "");

    bool isNumeric() const;
    bool isOrdinal() const;
    bool isSimple() const;
    bool isReal() const;
    bool isInteger() const;
    bool isBoolean() const;
    bool isChar() const;
    bool isString() const;
    bool isArray() const;
    bool isRecord() const;
    bool isSubrange() const;
    bool isEnumerated() const;

    std::string toString() const;
};

bool isCompatible(const TypeInfo& t1, const TypeInfo& t2, const SymbolTable& st);

bool isAssignmentCompatible(const TypeInfo& t1, const TypeInfo& t2, const SymbolTable& st);

TypeInfo resultType(const std::string& op,
                    const TypeInfo& left,
                    const TypeInfo& right,
                    const SymbolTable& st);

TypeInfo resultTypeUnary(const std::string& op,
                         const TypeInfo& operand);

TypeInfo typeInfoFromTab(int tabIdx, const SymbolTable& st);

bool isValidIndexType(const TypeInfo& t);

bool isValidSubrangeType(const TypeInfo& t);

bool isBooleanType(const TypeInfo& t);

bool isOrdinalType(const TypeInfo& t);

#endif