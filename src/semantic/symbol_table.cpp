#include "symbol_table.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

TabEntry::TabEntry() : identifier(""), link(0), obj(ObjClass::VARIABLE), type(TypeCode::NOTYPE), ref(0), nrm(1), lev(0), adr(0) {}

AtabEntry::AtabEntry() : xtyp(0), etyp(0), eref(0), low(0), high(0), elsz(1), size(0) {}

BtabEntry::BtabEntry() : last(0), lpar(0), psze(0), vsze(0) {}

SymbolTable::SymbolTable() : currentLevel(0), tabPtr(0), atabPtr(1), btabPtr(1)
{
    atab.push_back(AtabEntry());
    btab.push_back(BtabEntry());
    display.push_back(0);
    initReservedWords();
    initPredefined();
}

void SymbolTable::initReservedWords() {
    auto pushRW = [&](const std::string& id) {
        TabEntry e;
        e.identifier = id;
        e.link = 0;
        e.obj = ObjClass::TYPE;
        e.type = TypeCode::NOTYPE;
        e.ref = 0;
        e.nrm = 1;
        e.lev = 0;
        e.adr = 0;
        tab.push_back(e);
        tabPtr++;
    };
    pushRW("and");
    pushRW("array");
    pushRW("begin");
    pushRW("case");
    pushRW("const");
    pushRW("div");
    pushRW("downto");
    pushRW("do");
    pushRW("else");
    pushRW("end");
    pushRW("for");
    pushRW("function");
    pushRW("if");
    pushRW("mod");
    pushRW("not");
    pushRW("of");
    pushRW("or");
    pushRW("procedure");
    pushRW("program");
    pushRW("record");
    pushRW("repeat");
    pushRW("integer");
    pushRW("real");
    pushRW("boolean");
    pushRW("char");
    pushRW("string");
    pushRW("then");
    pushRW("to");
    pushRW("type");
    pushRW("until");
    pushRW("var");
    pushRW("while");
    pushRW("true");
    pushRW("false");
}

void SymbolTable::initPredefined() {
    auto pushType = [&](const std::string& id, TypeCode tc) {
        TabEntry e;
        e.identifier = id;
        e.link = btab[0].last;
        e.obj = ObjClass::TYPE;
        e.type = tc;
        e.ref = 0;
        e.nrm = 1;
        e.lev = 0;
        e.adr = 0;
        tab.push_back(e);
        btab[0].last = tabPtr;
        tabPtr++;
    };
    auto pushConst = [&](const std::string& id, TypeCode tc, int val) {
        TabEntry e;
        e.identifier = id;
        e.link = btab[0].last;
        e.obj = ObjClass::CONSTANT;
        e.type = tc;
        e.ref = 0;
        e.nrm = 1;
        e.lev = 0;
        e.adr = val;
        tab.push_back(e);
        btab[0].last = tabPtr;
        tabPtr++;
    };
    auto pushProc = [&](const std::string& id) {
        TabEntry e;
        e.identifier = id;
        e.link = btab[0].last;
        e.obj = ObjClass::PROCEDURE;
        e.type = TypeCode::NOTYPE;
        e.ref = 0;
        e.nrm = 1;
        e.lev = 0;
        e.adr = 0;
        tab.push_back(e);
        btab[0].last = tabPtr;
        tabPtr++;
    };
    pushType("Integer", TypeCode::INTEGER);
    pushType("Real", TypeCode::REAL);
    pushType("Char", TypeCode::CHAR);
    pushType("Boolean", TypeCode::BOOLEAN);
    pushType("String", TypeCode::STRING);
    pushConst("True", TypeCode::BOOLEAN, 1);
    pushConst("False", TypeCode::BOOLEAN, 0);
    pushProc("writeln");
    pushProc("readln");
    pushProc("write");
    pushProc("read");
}

int SymbolTable::enter_block() {
    currentLevel++;
    BtabEntry b;
    b.last = 0;
    b.lpar = 0;
    b.psze = 0;
    b.vsze = 0;
    btab.push_back(b);
    btabPtr++;
    int newBlockIdx = static_cast<int>(btab.size()) - 1;
    if (currentLevel < static_cast<int>(display.size())) {
        display[currentLevel] = newBlockIdx;
    } else {
        display.push_back(newBlockIdx);
    }
    return newBlockIdx;
}

void SymbolTable::exit_block() {
    if (currentLevel > 0) {
        currentLevel--;
    }
}

int SymbolTable::currentLev() const {
    return currentLevel;
}

int SymbolTable::currentBlock() const {
    return display[currentLevel];
}

int SymbolTable::insert(const std::string& id, ObjClass obj, TypeCode type, int ref, int nrm, int adr) {
    int blk = currentBlock();
    TabEntry e;
    e.identifier = id;
    e.link = btab[blk].last;
    e.obj = obj;
    e.type = type;
    e.ref = ref;
    e.nrm = nrm;
    e.lev = currentLevel;
    e.adr = adr;
    tab.push_back(e);
    int idx = tabPtr;
    btab[blk].last = idx;
    tabPtr++;
    if (obj == ObjClass::VARIABLE || obj == ObjClass::FIELD) {
        btab[blk].vsze += sizeOf(type);
    }
    return idx;
}

int SymbolTable::lookup(const std::string& id) const {
    std::string idLower = id;
    std::transform(idLower.begin(), idLower.end(), idLower.begin(), ::tolower);

    for (int lv = currentLevel; lv >= 0; lv--) {
        int blk = display[lv];
        int k = btab[blk].last;
        while (k != 0) {
            std::string tabIdLower = tab[k].identifier;
            std::transform(tabIdLower.begin(), tabIdLower.end(), tabIdLower.begin(), ::tolower);
            if (tabIdLower == idLower) return k;
            k = tab[k].link;
        }
    }

    for (int i = 0; i < 34 && i < (int)tab.size(); i++) {
        std::string tabIdLower = tab[i].identifier;
        std::transform(tabIdLower.begin(), tabIdLower.end(), tabIdLower.begin(), ::tolower);
        if (tabIdLower == idLower) return i;
    }

    return -1;
}

int SymbolTable::lookupCurrentBlock(const std::string& id) const {
    std::string idLower = id;
    std::transform(idLower.begin(), idLower.end(), idLower.begin(), ::tolower);
    int blk = currentBlock();
    int k = btab[blk].last;
    while (k != 0) {
        std::string tabIdLower = tab[k].identifier;
        std::transform(tabIdLower.begin(), tabIdLower.end(), tabIdLower.begin(), ::tolower);
        if (tabIdLower == idLower) return k;
        k = tab[k].link;
    }
    return -1;
}

int SymbolTable::insertArray(int xtyp, int etyp, int eref, int low, int high, int elsz) {
    AtabEntry a;
    a.xtyp = xtyp;
    a.etyp = etyp;
    a.eref = eref;
    a.low = low;
    a.high = high;
    a.elsz = elsz;
    a.size = (high - low + 1) * elsz;
    atab.push_back(a);
    int idx = atabPtr;
    atabPtr++;
    return idx;
}

int SymbolTable::sizeOf(TypeCode t) const {
    switch (t) {
        case TypeCode::INTEGER:  return 1;
        case TypeCode::REAL:     return 2;
        case TypeCode::CHAR:     return 1;
        case TypeCode::BOOLEAN:  return 1;
        case TypeCode::STRING:   return 4;
        case TypeCode::ARRAY:    return 0;
        case TypeCode::RECORD:   return 0;
        default:                 return 1;
    }
}

std::string typeCodeToString(TypeCode tc) {
    switch (tc) {
        case TypeCode::NOTYPE:     return "NOTYPE";
        case TypeCode::INTEGER:    return "INTEGER";
        case TypeCode::REAL:       return "REAL";
        case TypeCode::CHAR:       return "CHAR";
        case TypeCode::BOOLEAN:    return "BOOLEAN";
        case TypeCode::STRING:     return "STRING";
        case TypeCode::ARRAY:      return "ARRAY";
        case TypeCode::RECORD:     return "RECORD";
        case TypeCode::SUBRANGE:   return "SUBRANGE";
        case TypeCode::ENUMERATED: return "ENUMERATED";
        default:                   return "UNKNOWN";
    }
}

std::string objClassToString(ObjClass oc) {
    switch (oc) {
        case ObjClass::CONSTANT:  return "CONSTANT";
        case ObjClass::VARIABLE:  return "VARIABLE";
        case ObjClass::TYPE:      return "TYPE";
        case ObjClass::PROCEDURE: return "PROCEDURE";
        case ObjClass::FUNCTION:  return "FUNCTION";
        case ObjClass::PROGRAM:   return "PROGRAM";
        case ObjClass::FIELD:     return "FIELD";
        default:                  return "UNKNOWN";
    }
}