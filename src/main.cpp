#include "lexer/lexer.hpp"
#include "io/reader.hpp"
#include "io/writer.hpp"
#include "parser/parser.hpp"
#include "parser/utils/error.hpp"
#include "semantic/semantic_analyzer.hpp"
#include "backend/codegen/code_generator.hpp"

int main () {

    /* ========== LEXER ==========*/
    INIT_DICTIONARY();
    INPUT_FILE();
    READ_ALL_FILE();
    // PRINT_TOKEN_LIST();
    // SAVE_TOKEN_LIST();

    /* ========== PARSER ==========*/
    Node* root = nullptr;
    try{
        Parser parser(token_list);
        root = parser.parse();

        // PRINT_PARSE_TREE (root, 0);

        // SAVE_PARSE_TREE (root, 0);
    } catch (const SyntaxError& e){
        std::cerr << e.what() << std::endl;
        return 0;
    }

    /* ========== SEMANTIC ANALYSIS ==========*/
    if (root != nullptr) {
        SemanticContext ctx;
        analyze(root, ctx);

        // Kalo ada error ga bisa hasilin symbol table sama ast 
        if (ctx.errors.hasErrors()) {
            std::cout << "Error" << std::endl;
            ctx.errors.printAll();
            // PRINT_SYMBOL_TABLE(ctx.st);
        } else {
            std::cout << "Success" << std::endl;
            // PRINT_SYMBOL_TABLE(ctx.st);
            // SAVE_SYMBOL_TABLE(ctx.st);
            // Print dan save AST (format baru: indentation-based, tanpa box-drawing)
            // PRINT_AST(root, 0, ctx.st);
            // SAVE_AST(root, 0, ctx.st);
            // PRINT_AST_NEW();
            // SAVE_AST_NEW();

            std::cout << "\n=== INTERMEDIATE CODE ===\n";

            CodeGenerator cg(ctx.st);

            auto instructions = cg.generate(g_astRoot);

            for (const auto& ins : instructions) {
                std::cout << ins.toString() << std::endl;
            }
        }

        delete root;
    }

    return 0;
}