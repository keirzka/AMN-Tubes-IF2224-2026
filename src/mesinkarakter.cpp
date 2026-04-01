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
    char firstChar;
    fileStream.get(firstChar);

    if (fileStream.eof() || fileStream.fail()) {
        currentChar = '\0';
    }
    else {
        currentChar = firstChar;
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
    ADD_TOKEN("intcon", lexeme);
}

// void READ_STRING(){
//     string lexeme = "";
//     while (currentChar != '\0' && IS_LETTER(currentChar)) {
//         lexeme += currentChar;
//         ADV();
//     }
//     ADD_TOKEN("string", lexeme);
// }

// void READ_CHAR(){

// }

void READ_SPECIAL_TOKEN() {
    if (currentChar == '=') {
        ADV();
        if (currentChar == '=') {
            ADV();
            token_list.push_back("eql");
        } else {
            cout << "Lexical error: '=' tunggal tidak valid." << endl;
        }
    }
    else if (currentChar == '<') {
        ADV();
        if (currentChar == '>') {
            ADV();
            token_list.push_back("neq");
        } else if (currentChar == '=') {
            ADV();
            token_list.push_back("leq");
        } else {
            token_list.push_back("lss");
        }
    }
    else if (currentChar == '>') {
        ADV();
        if (currentChar == '=') {
            ADV();
            token_list.push_back("geq");
        } else {
            token_list.push_back("gtr");
        }
    }
    else if (currentChar == ':') {
        ADV();
        if (currentChar == '=') {
            ADV();
            token_list.push_back("becomes");
        } else {
            token_list.push_back("colon");
        }
    }
    else if (currentChar == '(') {
        ADV();
        token_list.push_back("lparent");
    }
    else if (currentChar == ')') {
        ADV();
        token_list.push_back("rparent");
    }
    else if (currentChar == '[') {
        ADV();
        token_list.push_back("lbrack");
    }
    else if (currentChar == ']') {
        ADV();
        token_list.push_back("rbrack");
    }
    else if (currentChar == ',') {
        ADV();
        token_list.push_back("comma");
    }
    else if (currentChar == ';') {
        ADV();
        token_list.push_back("semicolon");
    }
    else if (currentChar == '.') {
        ADV();
        token_list.push_back("period");
    }
    else if (currentChar == '+'){
        ADV();
        token_list.push_back("plus");
    }
    else if (currentChar == '-'){
        ADV();
        token_list.push_back("minus");
    }else if (currentChar == '*'){
        ADV();
        token_list.push_back("times");
    }
    else if (currentChar == '/'){
        ADV();
        token_list.push_back("rdiv");
    }
    else if (currentChar == 'div'){
        ADV();
        token_list.push_back("idiv");
    }
    else if (currentChar == 'MOD'){
        ADV();
        token_list.push_back("imod");
    }
    else if (currentChar == 'AND'){
        ADV();
        token_list.push_back("andsy");
    }
    else if (currentChar == 'OR'){
        ADV();
        token_list.push_back("orsy");
    }
    else {
        cout << "Lexical error: karakter tidak dikenal '" << currentChar << "'" << endl;
        ADV();
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