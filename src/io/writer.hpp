#ifndef __WRITER_HPP_
#define __WRITER_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "tree/node.hpp"
// #include "tree/ast_node.hpp"
#include "semantic/symbol_table.hpp"

using namespace std;

extern vector<string> token_list; // array dinamis berisi semua token (kata)

/**
 * @brief Meng-output-kan string-string dalam token_list ke terminal
 *
 * **I.S** : token_list terdefinisi
 *
 * **F.S** : token_list ter-output ke terminal 
 * 
 */
void PRINT_TOKEN_LIST ();

/**
 * @brief Meng-output-kan string-string dalam token_list ke terminal. Program meminta nama file output ke user
 *
 * **I.S** : token_list terdefinisi
 *
 * **F.S** : token_list ter-output ke file dengan nama yang diinput user
 * 
 */
void SAVE_TOKEN_LIST ();

/*
Print parse tree
*/

void PRINT_PARSE_TREE (Node* akar, int depth);

void SAVE_PARSE_TREE (Node* akar, int depth);

void SAVE_PARSE_TREE_RECCURSIVE (Node* akar, int depth, ofstream& os);

void PRINT_SYMBOL_TABLE(const SymbolTable& st);

void SAVE_SYMBOL_TABLE(const SymbolTable& st);

void PRINT_AST(Node* root, int depth, const SymbolTable& st);

void SAVE_AST(Node* root, int depth, const SymbolTable& st);

void SAVE_AST_RECURSIVE(Node* root, int depth, ofstream& os, const SymbolTable& st);

// New AST functions with better formatting (uses global g_astRoot)
// void PRINT_AST_NEW();

// void SAVE_AST_NEW();

#endif