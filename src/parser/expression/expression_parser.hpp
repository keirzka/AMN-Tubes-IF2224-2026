#ifndef __EXPRESSION_PARSER_HPP_
#define __EXPRESSION_PARSER_HPP_

#include "parser/utils/token_stream.hpp"
#include "tree/node.hpp"
#include "parser/statement/statement_parser.hpp"

Node* parseExpression(TokenStream& ts); // ✅
Node* parseSimpleExpression(TokenStream& ts); // ✅
Node* parseTerm(TokenStream& ts); // ✅
Node* parseVariableTanpaIdent (TokenStream& ts); // ✅
Node* parseProcedureCallTanpaIdent (TokenStream& ts); // ✅
Node* parseFactor(TokenStream& ts); // ✅
Node* parseVariable (TokenStream& ts); // ✅
Node* parseComponentVariable (TokenStream& ts); // ✅
Node* parseIndexList (TokenStream& ts); // ✅
// Node* parseAssignmentStatement (TokenStream& ts); // ✅
Node* parseParameterList (TokenStream& ts); // ✅
Node* parseRelationalOperator (TokenStream& ts); // ✅
Node* parseAdditiveOperator (TokenStream& ts); // ✅
Node* parseMultiplicativeOperator (TokenStream& ts); // ✅


#endif