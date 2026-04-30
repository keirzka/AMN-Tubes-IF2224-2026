#ifndef __WRITER_HPP_
#define __WRITER_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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
SAVE PARSE TREE
*/

#endif