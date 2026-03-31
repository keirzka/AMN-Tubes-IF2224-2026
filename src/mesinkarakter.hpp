#ifndef __MESIN_KAR_H_
#define __MESIN_KAR_H_

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "dictionary.hpp"

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

/* ===============================================================
 *  STATE MESIN
 * =============================================================== */

// State Karakter/Kata
#define BLANK_CHARACTER 2
#define NORMAL_CHARACTER 1
#define SPECIAL_CHARACTER 0


// Variabel global
extern char currentChar; // karakter yang dibaca saat ini
extern string currentWord; // kata yang terbentuk dari karakter-karakter saat ini
extern int currentWordState; // state kata
extern vector<string> token_list; // array dinamis berisi semua token (kata)
extern ifstream fileStream; // stream untuk membaca file



/* ===============================================================
 *  PRIMITIF MESIN KARAKTER
 * =============================================================== */


/**
 * @brief Membaca sebuah file. Program meminta user untuk memasukkan nama file hingga valid
 *
 * **I.S** : fileStream dalam keadaan close  
 * **F.S** : 
 * - `fileStream` masuk ke keadaan open
 * 
 * 
 */
void INPUT_FILE ();


/**
 * @brief Membaca karakter pertama dari fileStream
 *
 * **I.S** : fileStream dalam keadaan open  
 * **F.S** : 
 * - `currentChar` berisi karakter pertama dari file (atau `'\0'` jika EOF).
 * - `currentWordState` berisi CHARACTER_STATE() dari `currentChar`
 * 
 */
void START_FILE();


/**
 * @brief Membaca karakter berikutnya.
 *
 * **I.S** :  
 * - Mesin sudah diinisialisasi dengan START_FILE.  
 * - `currentChar` berisi karakter saat ini.  
 *
 * **F.S** :  
 * - Jika masih ada karakter: `currentChar` menjadi karakter berikutnya.  
 * - Jika mencapai EOF:  
 *     - `currentChar = '\\0'`
 */
void ADV();

/**
 * @brief Fungsi mengembalikan state dari currentChar
 */
int CHARACTER_STATE();


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

string TO_LOWER(const string& s);
void ADD_TOKEN(const string& tokenType, const string& lexeme = "");

#endif