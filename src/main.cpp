#include "lexer/lexer.hpp"
#include "io/reader.hpp"
#include "io/writer.hpp"
#include "parser/parser.hpp"
#include "parser/utils/error.hpp"

// std::vector<std::string> token_list;
// Node* root;

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

        PRINT_PARSE_TREE (root, 0);

        SAVE_PARSE_TREE (root, 0);

        delete root;
    } catch (const SyntaxError& e){
        std::cerr << e.what() <<std::endl;
        return 1;
    }

    return 0;
}