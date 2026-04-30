#ifndef __DECLARATION_PARSER_HPP_
#define __DECLARATION_PARSER_HPP_

#include "parser/utils/token_stream.hpp"
#include "tree/node.hpp"

// TODO :
Node* parseConstDeclaration(TokenStream& ts);

Node* parseTypeDeclaration(TokenStream& ts);

Node* parseVarDeclaration(TokenStream& ts);

Node* parseSubprogramDeclaration(TokenStream& ts);
#endif