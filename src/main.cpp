#include "lexer/lexer.hpp"
#include "io/reader.hpp"
#include "io/writer.hpp"
#include "parser/parser.hpp"
#include "parser/utils/error.hpp"

extern std::vector<std::string> token_list;

int main () {

    /* ========== LEXER ==========*/
    INIT_DICTIONARY();
    INPUT_FILE();
    READ_ALL_FILE();
    PRINT_TOKEN_LIST();
    SAVE_TOKEN_LIST();

    /* ========== PARSER ==========*/
    try{
        Parser parser(token_list);
        Node* root = parser.parse();

        // TODO : print tree

        // TODO : save tree to file

        delete root;
    } catch (const SyntaxError& e){
        std::cerr << e.what() <<std::endl;
        return 1;
    }

    return 0;
}