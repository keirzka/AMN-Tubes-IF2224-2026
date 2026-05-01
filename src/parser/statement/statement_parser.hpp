#ifndef __STATEMENT_PARSER_HPP_
#define __STATEMENT_PARSER_HPP_

#include "parser/utils/token_stream.hpp"
#include "tree/node.hpp"

/**
 * <compound-statement> → beginsy <statement-list> endsy
 */
Node* parseCompoundStatement(TokenStream& ts);

/**
 * <statement-list> → <statement> (semicolon <statement>)*
 */
Node* parseStatementList(TokenStream& ts);

/**
 * <statement> →
 *      <assignment>
 *    | <if-statement>
 *    | <while-statement>
 *    | <repeat-statement>
 *    | <for-statement>
 *    | <case-statement>
 *    | <procedure-call>
 *    | <compound-statement>
 *    | ε   (empty statement)
 */
Node* parseStatement(TokenStream& ts);

/**
 * <assignment> → ident becomes <expression>
 */
Node* parseAssignment(TokenStream& ts);

/**
 * <procedure-call> → ident (lparent <expression-list>? rparent)?
 */
Node* parseProcedureCall(TokenStream& ts);

/**
 * <if-statement> →
 *      ifsy <expression> thensy <statement>
 *      (elsesy <statement>)?
 */
Node* parseIfStatement(TokenStream& ts);

/**
 * <while-statement> →
 *      whilesy <expression> dosy <statement>
 */
Node* parseWhileStatement(TokenStream& ts);

/**
 * <repeat-statement> →
 *      repeatsy <statement-list> untilsy <expression>
 */
Node* parseRepeatStatement(TokenStream& ts);

/**
 * <for-statement> →
 *      forsy ident becomes <expression>
 *      (tosy | downtosy)
 *      <expression> dosy <statement>
 */
Node* parseForStatement(TokenStream& ts);

/**
 * <case-statement> →
 *      casesy <expression> ofsy <case-list> endsy
 */
Node* parseCaseStatement(TokenStream& ts);

/**
 * <case-list> →
 *      <case-branch> (semicolon <case-branch>)*
 */
Node* parseCaseList(TokenStream& ts);

/**
 * <case-branch> →
 *      <constant-list> colon <statement>
 */
Node* parseCaseBranch(TokenStream& ts);

/**
 * <constant-list> →
 *      <constant> (comma <constant>)*
 */
Node* parseConstantList(TokenStream& ts);

/**
 * <expression-list> →
 *      <expression> (comma <expression>)*
 */
Node* parseExpressionList(TokenStream& ts);

#endif