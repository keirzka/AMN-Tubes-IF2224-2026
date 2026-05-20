#include "writer.hpp"
#include <iomanip>
#include <algorithm>

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

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

    ofstream outputStream("test/milestone-3/" + fileName + ".txt");
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


void PRINT_SYMBOL_TABLE(const SymbolTable& st) {
    // TAB 
    cout << "[tab]" << endl;
    cout << setw(6) << "Index"
         << setw(20) << "Identifier"
         << setw(15) << "Object"
         << setw(15) << "Type"
         << setw(10) << "Ref"
         << setw(8) << "Nrm"
         << setw(8) << "Lev"
         << setw(10) << "Address" << endl;

    for (int i = 0; i < (int)st.tab.size(); i++) {
        const TabEntry& entry = st.tab[i];
        cout << setw(6) << i
             << setw(20) << entry.identifier.substr(0, 19)
             << setw(15) << objClassToString(entry.obj)
             << setw(15) << typeCodeToString(entry.type)
             << setw(10) << entry.ref
             << setw(8) << entry.nrm
             << setw(8) << entry.lev
             << setw(10) << entry.adr << endl;
    }

    // ATAB 
    cout << "[atab]" << endl;
    cout << setw(6) << "Index"
         << setw(15) << "Index Type"
         << setw(15) << "Element Type"
         << setw(10) << "ERef"
         << setw(8) << "Low"
         << setw(8) << "High"
         << setw(10) << "ElSize"
         << setw(10) << "Size" << endl;

    for (int i = 0; i < (int)st.atab.size(); i++) {
        const AtabEntry& entry = st.atab[i];
        cout << setw(6) << i
             << setw(15) << typeCodeToString((TypeCode)entry.xtyp)
             << setw(15) << typeCodeToString((TypeCode)entry.etyp)
             << setw(10) << entry.eref
             << setw(8) << entry.low
             << setw(8) << entry.high
             << setw(10) << entry.elsz
             << setw(10) << entry.size << endl;
    }

    // BTAB 
    cout << "[btab]" << endl;
    cout << setw(6) << "Index"
         << setw(15) << "Last"
         << setw(15) << "Last Param"
         << setw(15) << "Param Size"
         << setw(15) << "Var Size" << endl;

    for (int i = 0; i < (int)st.btab.size(); i++) {
        const BtabEntry& entry = st.btab[i];
        cout << setw(6) << i
             << setw(15) << entry.last
             << setw(15) << entry.lpar
             << setw(15) << entry.psze
             << setw(15) << entry.vsze << endl;
    }

}

void SAVE_SYMBOL_TABLE(const SymbolTable& st) {
    cout << "Masukkan nama file untuk menyimpan symbol table : ";
    string fileName;
    getline(cin, fileName);

    ofstream outputStream("test/milestone-3/" + fileName + ".txt");
    if (!outputStream.is_open()) {
        cerr << "Error: Tidak dapat membuka file untuk penulisan" << endl;
        return;
    }

    // TAB 

    outputStream << "[tab]" << endl;
    outputStream << setw(6) << "Index"
                 << setw(20) << "Identifier"
                 << setw(15) << "Object"
                 << setw(15) << "Type"
                 << setw(10) << "Ref"
                 << setw(8) << "Nrm"
                 << setw(8) << "Lev"
                 << setw(10) << "Address" << endl;

    for (int i = 0; i < (int)st.tab.size(); i++) {
        const TabEntry& entry = st.tab[i];
        outputStream << setw(6) << i
                     << setw(20) << entry.identifier.substr(0, 19)
                     << setw(15) << objClassToString(entry.obj)
                     << setw(15) << typeCodeToString(entry.type)
                     << setw(10) << entry.ref
                     << setw(8) << entry.nrm
                     << setw(8) << entry.lev
                     << setw(10) << entry.adr << endl;
    }

    // ATAB 
    outputStream << "[atab]" << endl;
    outputStream << setw(6) << "Index"
                 << setw(15) << "Index Type"
                 << setw(15) << "Element Type"
                 << setw(10) << "ERef"
                 << setw(8) << "Low"
                 << setw(8) << "High"
                 << setw(10) << "ElSize"
                 << setw(10) << "Size" << endl;

    for (int i = 0; i < (int)st.atab.size(); i++) {
        const AtabEntry& entry = st.atab[i];
        outputStream << setw(6) << i
                     << setw(15) << typeCodeToString((TypeCode)entry.xtyp)
                     << setw(15) << typeCodeToString((TypeCode)entry.etyp)
                     << setw(10) << entry.eref
                     << setw(8) << entry.low
                     << setw(8) << entry.high
                     << setw(10) << entry.elsz
                     << setw(10) << entry.size << endl;
    }

    // BTAB 
    outputStream << "[btab]" << endl;
    outputStream << setw(6) << "Index"
                 << setw(15) << "Last"
                 << setw(15) << "Last Param"
                 << setw(15) << "Param Size"
                 << setw(15) << "Var Size" << endl;

    for (int i = 0; i < (int)st.btab.size(); i++) {
        const BtabEntry& entry = st.btab[i];
        outputStream << setw(6) << i
                     << setw(15) << entry.last
                     << setw(15) << entry.lpar
                     << setw(15) << entry.psze
                     << setw(15) << entry.vsze << endl;
    }

    outputStream.close();
}

void PRINT_AST(Node* root, int depth, const SymbolTable& st) {
    if (!root) return;

    cout << string(depth*4, ' ');
    cout << root->getLabel();

    if (root->isAnnotated()) {
        cout << " [type=" << typeCodeToString((TypeCode)root->sem_type)
             << ", ref=" << root->sem_tab_index
             << ", lev=" << root->sem_lev << "]";
    }

    cout << endl;

    for (auto child : root->children) {
        PRINT_AST(child, depth + 1, st);
    }
}

void SAVE_AST(Node* root, int depth, const SymbolTable& st) {
    cout << "Masukkan nama file untuk menyimpan decorated AST : ";
    string fileName;
    getline(cin, fileName);

    ofstream outputStream("test/milestone-3/" + fileName + ".txt");
    if (!outputStream.is_open()) {
        cerr << "Error: Tidak dapat membuka file untuk penulisan" << endl;
        return;
    }

    SAVE_AST_RECURSIVE(root, depth, outputStream, st);

    outputStream << "\n" << string(80, '=') << endl;
    outputStream.close();
}

// New AST functions using global g_astRoot
// extern std::shared_ptr<ProgramNode> g_astRoot;

// void PRINT_AST_NEW() {
//     if (!g_astRoot) {
//         cout << "AST tidak tersedia" << endl;
//         return;
//     }
//     g_astRoot->print(0, false, false);
// }

// void SAVE_AST_NEW() {
//     if (!g_astRoot) {
//         cout << "AST tidak tersedia" << endl;
//         return;
//     }

//     cout << "Masukkan nama file untuk menyimpan AST : ";
//     string fileName;
//     getline(cin, fileName);

//     ofstream outputStream("test/milestone-3/" + fileName + ".txt");
//     if (!outputStream.is_open()) {
//         cerr << "Error: Tidak dapat membuka file untuk penulisan" << endl;
//         return;
//     }

//     // Save current cout buffer
//     std::streambuf* old = cout.rdbuf(outputStream.rdbuf());
//     g_astRoot->print(0, false, false);
//     cout.rdbuf(old);

//     outputStream << "\n" << string(80, '=') << endl;
//     outputStream.close();
// }

void SAVE_AST_RECURSIVE(Node* root, int depth, ofstream& os, const SymbolTable& st) {
    if (!root) return;

    os << string(depth*4, ' ');
    os << root->getLabel();

    if (root->isAnnotated()) {
        os << " [type=" << typeCodeToString((TypeCode)root->sem_type)
           << ", ref=" << root->sem_tab_index
           << ", lev=" << root->sem_lev << "]";
    }

    os << endl;

    for (auto child : root->children) {
        SAVE_AST_RECURSIVE(child, depth + 1, os, st);
    }
}