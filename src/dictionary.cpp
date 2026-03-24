#include "dictionary.hpp"

// Definisi global dictionary
map<string, string> dictionary;

void INIT_DICTIONARY() {
    dictionary["NOT"] = "notsy";
    dictionary["+"] = "plus";
    dictionary["-"] = "minus";
    dictionary["*"] = "times";
    dictionary["div"] = "idiv";
    dictionary["/"] = "rdiv";
    dictionary["MOD"] = "imod";
    dictionary["AND"] = "andsy";
    dictionary["OR"] = "orsy";
    dictionary["=="] = "eql";
    dictionary["<>"] = "neq";
    dictionary[">"] = "gtr";
    dictionary[">="] = "geq";
    dictionary["<"] = "lss";
    dictionary["<="] = "leq";
    dictionary["("] = "lparent";
    dictionary[")"] = "rparent";
    dictionary["["] = "lbrack";
    dictionary["]"] = "rbrack";
    dictionary[","] = "comma";
    dictionary[";"] = "semicolon";
    dictionary["."] = "period";
    dictionary[":"] = "colon";
    dictionary[":="] = "becomes";
    dictionary["const"] = "constsy";
    dictionary["type "] = "typesy";
    dictionary["var"] = "varsy";
    dictionary["function"] = "functionsy";
    dictionary["procedure"] = "proceduresy";
    dictionary["array"] = "arraysy";
    dictionary["record"] = "recordsy";
    dictionary["program"] = "programsy";
    dictionary["begin"] = "beginsy";
    dictionary["if"] = "ifsy";
    dictionary["case"] = "casesy";
    dictionary["repeat"] = "repeatsy";
    dictionary["while"] = "whilesy";
    dictionary["for"] = "forsy";
    dictionary["end"] = "endsy";
    dictionary["else"] = "elsesy";
    dictionary["until"] = "untilsy";
    dictionary["of"] = "ofsy";
    dictionary["do"] = "dosy";
    dictionary["to"] = "tosy";
    dictionary["downto"] = "downtosy";
    dictionary["then"] = "thensy";
}