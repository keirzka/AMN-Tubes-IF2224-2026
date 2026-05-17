#include "writer.hpp"

void PRINT_TOKEN_LIST () 
{
    for (int i = 0; i < (int)token_list.size(); i++) {
        cout << token_list[i] << endl;
    }
}

void SAVE_TOKEN_LIST () 
{
    cout << "Masukkan nama file untuk menyimpan hasil token list : ";
    string fileName;
    getline(cin, fileName);

    ofstream outputStream("test/milestone-3/" + fileName + ".txt");
    if (!outputStream.is_open()) {
        cerr << "Error: Tidak dapat membuka file untuk penulisan" << endl;
        return;
    }

    for (int i = 0; i < (int)token_list.size(); i++) {
        outputStream << token_list[i] << endl;
    }
    outputStream.close();
}

void PRINT_PARSE_TREE (Node* akar, int depth) {
    cout << string(depth*4, ' ');
    cout << akar->getLabel() << endl;

    for (auto anak : akar->children) {
        PRINT_PARSE_TREE (anak, depth + 1);
    }
}

void SAVE_PARSE_TREE (Node* akar, int depth) {
    cout << "Masukkan nama file untuk menyimpan hasil parse tree : ";
    string fileName;
    getline(cin, fileName);

    ofstream outputStream("test/milestone-2/" + fileName + ".txt");
    if (!outputStream.is_open()) {
        cerr << "Error: Tidak dapat membuka file untuk penulisan" << endl;
        return;
    }
    
    SAVE_PARSE_TREE_RECCURSIVE(akar, depth, outputStream);
    outputStream.close();
}

void SAVE_PARSE_TREE_RECCURSIVE (Node* akar, int depth, ofstream& os) {
    os << string(depth*4, ' ');
    os << akar->getLabel() << endl;

    for (auto anak : akar->children) {
        SAVE_PARSE_TREE_RECCURSIVE(anak, depth + 1, os);
    }
}