#ifndef __EXPRESSION_PARSER_HPP_
#define __EXPRESSION_PARSER_HPP_

#include "parser/utils/token_stream.hpp"
#include "tree/node.hpp"

/**
 * IMPLEMENTASI MINIMAL (TIDAK FULL GRAMMAR)
 * 
 * Sudah mengikuti struktur:
 * <expression> → <simple-expression>
 * <simple-expression> → <term> (addop <term>)*
 * <term> → <factor> (mulop <factor>)*
 * <factor> → ident | number | (expression)
 * 
 * TODO (Fakhry):
 * - Tambah relop
 * - Tambah NOT, unary minus
 * - Tambah boolean expression
 */

Node* parseExpression(TokenStream& ts);
Node* parseSimpleExpression(TokenStream& ts);
Node* parseTerm(TokenStream& ts);
Node* parseFactor(TokenStream& ts);

#endif