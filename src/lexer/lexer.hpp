#ifndef __LEXER_HPP_
#define __LEXER_HPP_

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include "dictionary.hpp"

extern vector<string> token_list; // array dinamis berisi semua token (kata)

using namespace std;

/**
 * @file mesinkarakter.h
 * @brief Mesin karakter untuk membaca input satu per satu karakter,
 *        baik dari stdin maupun dari file.
 *
 * Mesin karakter menyediakan operasi dasar:
 * - Inisialisasi pembacaan (file)
 * - Pembacaan karakter berikutnya (ADV)
 * - Indikator akhir pembacaan (eof)
 *
 * 
 */

// State Karakter/Kata
#define BLANK_CHARACTER 2
#define NORMAL_CHARACTER 1
#define SPECIAL_CHARACTER 0

/**
 * @brief Fungsi mengembalikan state dari currentChar
 */
int CHARACTER_STATE();

/**
 * @brief Membaca semua karakter dari fileStream
 *
 * **I.S** : fileStream dalam keadaan open  
 * **F.S** : 
 * - `fileStream` sampai pada eof
 * 
 */
void READ_ALL_FILE ();

bool IS_LETTER(char c);
bool IS_DIGIT(char c);
bool IS_ALNUM(char c);
bool IS_WHITESPACE(char c);

void SKIP_WHITESPACE();
void READ_IDENTIFIER_OR_KEYWORD();
void READ_NUMBER();
void READ_SPECIAL_TOKEN();
void READ_STRING_OR_CHAR();
void SKIP_COMMENT();

string TO_LOWER(const string& s);
void ADD_TOKEN(const string& tokenType, const string& lexeme = "");

#endif