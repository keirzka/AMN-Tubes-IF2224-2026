// parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include "tree/node.hpp"
#include "parser/utils/token_stream.hpp"
#include "parser/core/program_parser.hpp"

class Parser {
private:
    TokenStream stream;

public:
    // Constructor menerima token list dari lexer
    Parser(const std::vector<std::string>& tokens);

    // Jalankan parsing, return root parse tree
    // Throws SyntaxError jika ada kesalahan syntax
    Node* parse();
};

#endif