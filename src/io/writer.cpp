#include "writer.hpp"

void PRINT_TOKEN_LIST () 
{
    for (int i = 0; i < (int)token_list.size(); i++) {
        cout << token_list[i] << endl;
    }
}

void SAVE_TOKEN_LIST () 
{
    cout << "Masukkan nama file untuk menyimpan hasil : ";
    string fileName;
    getline(cin, fileName);

    ofstream outputStream("test/milestone-1/" + fileName + ".txt");
    for (int i = 0; i < (int)token_list.size(); i++) {
        outputStream << token_list[i] << endl;
    }
}