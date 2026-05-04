// program_parser.hpp
#ifndef PROGRAM_PARSER_HPP
#define PROGRAM_PARSER_HPP

#include "parser/utils/token_stream.hpp"
#include "tree/node.hpp"
#include "parser/declaration/declaration_parser.hpp"
#include "parser/statement/statement_parser.hpp"

Node* parseProgram(TokenStream& ts);
Node* parseProgramHeader(TokenStream& ts);
Node* parseDeclarationPartCoordinator(TokenStream& ts); // koordinasi const/type/var/subprogram
Node* parseCompoundStatementCoordinator(TokenStream& ts); 

#endif