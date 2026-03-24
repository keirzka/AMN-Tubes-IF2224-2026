#include "mesinkarakter.hpp"

// Variabel Global
char currentChar;
string currentWord;
int currentWordState;
vector<string> token_list;
ifstream fileStream; 

void INPUT_FILE () 
{
    while (true) {
        cout << "Masukkan nama file input : ";
        string fileName;
        getline(cin, fileName);

        ifstream fInput ("test/milestone-1/" + fileName);

        // cout << endl;
        if (!fInput.is_open()) {
            cout << "File dengan nama: " << fileName << " tidak ditemukan!" << endl;
        }
        else {
            fileStream.open("test/milestone-1/" + fileName);
            break;
        }
    }

}


void START_FILE()
{
    char firstChar;
    fileStream.get(firstChar);

    if (fileStream.eof() || fileStream.fail()) {
        currentChar = '\0';
    }
    else {
        currentChar = firstChar;
        currentWord = firstChar;
        currentWordState = CHARACTER_STATE();
    }
}

void ADV()
{
    char nextChar;
    fileStream.get(nextChar);

    if (fileStream.eof() || fileStream.fail()) {
        currentChar = '\0';
    }
    else {
        currentChar = nextChar;
    }
    
}

int CHARACTER_STATE () 
{
    if ((currentChar >= 'a' && currentChar <= 'z')
    || (currentChar >= 'A' && currentChar <= 'Z')
    || (currentChar >= '0' && currentChar <= '9')) {
        return NORMAL_CHARACTER;
    }
    else if (currentChar == ' '
    || currentChar == '\n'
    || currentChar == '\r') {
        return BLANK_CHARACTER;
    }
    else {
        return SPECIAL_CHARACTER;
    }
}

void PRINT_TOKEN_LIST () 
{
    for (int i = 0; i < token_list.size(); i++) {
        cout << token_list[i] << endl;
    }
}

void SAVE_TOKEN_LIST () 
{
    cout << "Masukkan nama file untuk menyimpan hasil : ";
    string fileName;
    getline(cin, fileName);
    // cout << endl;
    ofstream outputStream ("test/milestone-1/" + fileName + ".txt");
    for (int i = 0; i < token_list.size(); i++) {
        outputStream << token_list[i] << endl;
    }
}

void READ_ALL_FILE () 
{
    START_FILE();

    while (!fileStream.eof()) {
        ADV();
        if (CHARACTER_STATE() == currentWordState) {
            currentWord += currentChar;
        }
        else {
            if (currentWordState != BLANK_CHARACTER) {
                token_list.push_back(currentWord);
            }
            currentWord = currentChar;
            currentWordState = CHARACTER_STATE();
        }
    }
}