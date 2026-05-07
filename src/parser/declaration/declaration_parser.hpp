#ifndef DECLARATION_PARSER_HPP
#define DECLARATION_PARSER_HPP
#include "parser/utils/token_stream.hpp"
#include "tree/node.hpp"

Node* parseConstDeclaration(TokenStream& ts);
Node* parseTypeDeclaration(TokenStream& ts);
Node* parseVarDeclaration(TokenStream& ts);
Node* parseSubprogramDeclaration(TokenStream& ts);
Node* parseConstant(TokenStream& ts);
Node* parseIdentifierList(TokenStream& ts);
Node* parseType(TokenStream& ts);
Node* parseArrayType(TokenStream& ts);
Node* parseRange(TokenStream& ts);
Node* parseEnumerated(TokenStream& ts);
Node* parseRecordType(TokenStream& ts);
Node* parseFieldList(TokenStream& ts);
Node* parseFieldPart(TokenStream& ts);
Node* parseFormalParameterList(TokenStream& ts);
Node* parseParameterGroup(TokenStream& ts);
Node* parseBlock(TokenStream& ts);

#endif