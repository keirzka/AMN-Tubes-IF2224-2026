#include "lexer.hpp"

int main () {
    INIT_DICTIONARY();
    INPUT_FILE();
    READ_ALL_FILE();
    PRINT_TOKEN_LIST();
    SAVE_TOKEN_LIST();
}