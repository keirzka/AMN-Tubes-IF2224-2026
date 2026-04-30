#ifndef __READER_HPP_
#define __READER_HPP_

#include <fstream>
#include <sstream>
using namespace std;

extern char currentChar;

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

/*
PEEK : return next character dari current character
*/
char peek();

#endif