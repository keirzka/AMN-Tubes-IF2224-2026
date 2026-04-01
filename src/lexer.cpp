#include "lexer.hpp"

// Variabel Global
char currentChar;
vector<string> token_list;
ifstream fileStream; 

void INPUT_FILE () 
{
    while (true) {
        cout << "Masukkan nama file input : ";
        string fileName;
        getline(cin, fileName);

        ifstream fInput("test/milestone-1/" + fileName);

        if (!fInput.is_open()) {
            cout << "File dengan nama: " << fileName << " tidak ditemukan!" << endl;
        }
        else {
            fInput.close();
            fileStream.open("test/milestone-1/" + fileName);
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

int CHARACTER_STATE () 
{
    if ((currentChar >= 'a' && currentChar <= 'z')
    || (currentChar >= 'A' && currentChar <= 'Z')
    || (currentChar >= '0' && currentChar <= '9')) {
        return NORMAL_CHARACTER;
    }
    else if (currentChar == ' '
    || currentChar == '\n'
    || currentChar == '\r'
    || currentChar == '\t') {
        return BLANK_CHARACTER;
    }
    else {
        return SPECIAL_CHARACTER;
    }
}

bool IS_LETTER(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IS_DIGIT(char c) {
    return (c >= '0' && c <= '9');
}

bool IS_ALNUM(char c) {
    return IS_LETTER(c) || IS_DIGIT(c);
}

bool IS_WHITESPACE(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

string TO_LOWER(const string& s) {
    string res = s;
    for (int i = 0; i < (int)res.length(); i++) {
        if (res[i] >= 'A' && res[i] <= 'Z') {
            res[i] = res[i] - 'A' + 'a';
        }
    }
    return res;
}

void ADD_TOKEN(const string& tokenType, const string& lexeme) {
    if (lexeme == "") {
        token_list.push_back(tokenType);
    } else {
        token_list.push_back(tokenType + " (" + lexeme + ")");
    }
}

void SKIP_WHITESPACE() {
    while (currentChar != '\0' && IS_WHITESPACE(currentChar)) {
        ADV();
    }
}

void READ_IDENTIFIER_OR_KEYWORD() {
    string lexeme = "";
    while (currentChar != '\0' && IS_ALNUM(currentChar)) {
        lexeme += currentChar;
        ADV();
    }
    string lowerLexeme = TO_LOWER(lexeme);
    if (dictionary.find(lowerLexeme) != dictionary.end()) {
        token_list.push_back(dictionary[lowerLexeme]);
    } else {
        ADD_TOKEN("ident", lexeme);
    }
}

void READ_NUMBER() {
    string lexeme = "";

    while (currentChar != '\0' && IS_DIGIT(currentChar)) {
        lexeme += currentChar;
        ADV();
    }

    if (currentChar == '.') {
        ADV();

        if (IS_DIGIT(currentChar)) {
            lexeme += '.';

            while (currentChar != '\0' && IS_DIGIT(currentChar)) {
                lexeme += currentChar;
                ADV();
            }

            ADD_TOKEN("realcon", lexeme);
            return;
        } 
        else {
            ADD_TOKEN("intcon", lexeme);
            ADD_TOKEN("period", "");
            return;
        }
    }

    ADD_TOKEN("intcon", lexeme);
}

void READ_STRING_OR_CHAR() {
    string lexeme = "";
    ADV(); 

    while (currentChar != '\0') {
        if (currentChar == '\'') {
            if (peek() == '\'') { // escape '
                ADV(); // ke '
                lexeme += '\'';
                ADV(); // lanjut setelah escape
            } 
            else { // ' sebagai closing
                ADV();
                if (lexeme.length() == 1) {
                    ADD_TOKEN("charcon", lexeme);
                } else {
                    ADD_TOKEN("string", lexeme);
                }
                return;
            }
        }

        else if (currentChar == '\n') {
            cout << "Lexical error: string tidak boleh multiline\n";
            ADD_TOKEN("unknown", lexeme);
            return;
        }

        else {
            lexeme += currentChar;
            ADV();
        }
    }

    cout << "Lexical error: string tidak ditutup\n";
    ADD_TOKEN("unknown", lexeme);
}

char peek() {
    if (fileStream.eof()) return '\0';
    return fileStream.peek();
}

void SKIP_COMMENT_CURLY() {
    ADV(); 

    while (currentChar != '\0') {
        if (currentChar == '}') {
            ADV();
            return;
        }
        // else if (currentChar == '(') { 
        //     return;
        // }
        ADV();
    }

    cout << "Lexical error: comment tidak ditutup\n";
}

void SKIP_COMMENT_PAREN() {
    ADV(); 

    while (currentChar != '\0') {
        if (currentChar == '*') {
            ADV();
            if (currentChar == ')') {
                ADV();
                return;
            }
        }
        else if (currentChar == '{') { 
            return;
        }
        else {
            ADV();
        }
    }

    cout << "Lexical error: comment tidak ditutup\n";
}

void READ_SPECIAL_TOKEN() {

    if (currentChar == '\'') {
        READ_STRING_OR_CHAR();
        return;
    }

    else if (currentChar == '{') {
        SKIP_COMMENT_CURLY();
        return;
    }

    else if (currentChar == '(') {
        if (peek() == '*') { // cek next dulu
            ADV(); // skip (
            ADV(); // skip *
            SKIP_COMMENT_PAREN();
            return;
        } else {
            ADD_TOKEN("lparent", "");
            ADV();
            return;
        }
    }

    else if (currentChar == '=') {
        ADV();
        if (currentChar == '=') {
            ADV();
            ADD_TOKEN("eql", "");
        } else {
            ADD_TOKEN("unknown", "=");
        }
        return;
    }

    else if (currentChar == '<') {
        ADV();
        if (currentChar == '>') {
            ADV();
            ADD_TOKEN("neq", "");
        } else if (currentChar == '=') {
            ADV();
            ADD_TOKEN("leq", "");
        } else {
            ADD_TOKEN("lss", "");
        }
        return;
    }

    else if (currentChar == '>') {
        ADV();
        if (currentChar == '=') {
            ADV();
            ADD_TOKEN("geq", "");
        } else {
            ADD_TOKEN("gtr", "");
        }
        return;
    }

    else if (currentChar == ':') {
        ADV();
        if (currentChar == '=') {
            ADV();
            ADD_TOKEN("becomes", "");
        } else {
            ADD_TOKEN("colon", "");
        }
        return;
    }

    else if (currentChar == '+') { ADV(); ADD_TOKEN("plus", ""); return; }
    else if (currentChar == '-') { ADV(); ADD_TOKEN("minus", ""); return; }
    else if (currentChar == '*') { ADV(); ADD_TOKEN("times", ""); return; }
    else if (currentChar == '/') { ADV(); ADD_TOKEN("rdiv", ""); return; }
    else if (currentChar == ')') { ADV(); ADD_TOKEN("rparent", ""); return; }
    else if (currentChar == '[') { ADV(); ADD_TOKEN("lbrack", ""); return; }
    else if (currentChar == ']') { ADV(); ADD_TOKEN("rbrack", ""); return; }
    else if (currentChar == ',') { ADV(); ADD_TOKEN("comma", ""); return; }
    else if (currentChar == ';') { ADV(); ADD_TOKEN("semicolon", ""); return; }
    else if (currentChar == '.') { ADV(); ADD_TOKEN("period", ""); return; }

    else {
        string tmp(1, currentChar);
        ADD_TOKEN("unknown", tmp);
        ADV();
        return;
    }
}

void READ_ALL_FILE () 
{
    START_FILE();

    while (currentChar != '\0') {
        if (IS_WHITESPACE(currentChar)) {
            SKIP_WHITESPACE();
        }
        else if (IS_LETTER(currentChar)) {
            READ_IDENTIFIER_OR_KEYWORD();
        }
        else if (IS_DIGIT(currentChar)) {
            READ_NUMBER();
        }
        else {
            READ_SPECIAL_TOKEN();
        }
    }
}

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