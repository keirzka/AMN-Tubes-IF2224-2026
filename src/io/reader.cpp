#include "reader.hpp"
#include <iostream>

char currentChar;
ifstream fileStream; 

void INPUT_FILE () 
{
    while (true) {
        cout << "Masukkan nama file input : ";
        string fileName;
        getline(cin, fileName);

        ifstream fInput("test/milestone-3/" + fileName);

        if (!fInput.is_open()) {
            cout << "File dengan nama: " << fileName << " tidak ditemukan!" << endl;
        }
        else {
            fInput.close();
            fileStream.open("test/milestone-3/" + fileName);
            break;
        }
    }
}

void START_FILE()
{
    fileStream.get(currentChar);
    if (fileStream.eof() || fileStream.fail()) currentChar = '\0';
}

void ADV()
{
    fileStream.get(currentChar);
    if (fileStream.eof() || fileStream.fail()) currentChar = '\0';
}

char peek() {
    if (fileStream.eof()) return '\0';
    return fileStream.peek();
}