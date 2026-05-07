#include "parser.hpp"

Parser::Parser(const std::vector<std::string>& tokens) : stream(tokens) {}

// Jalankan parsing, return root parse tree
// Throws SyntaxError jika ada kesalahan syntax
Node* Parser::parse(){
    return parseProgram(stream);
}