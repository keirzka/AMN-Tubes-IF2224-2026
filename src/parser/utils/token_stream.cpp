#include "token_stream.hpp"
#include "error.hpp"

// Inisialisasi dengan token list dari lexer
TokenStream::TokenStream(const std::vector<std::string>& tokenList){
    this->tokens = tokenList;
    if(tokenList.size() > 0){
        this->currentIndex = 0;
    }
    else{
        this->currentIndex = -1;
    }
}

// Kembalikan token saat ini (tanpa maju)
std::string TokenStream::current() const{
    return this->tokens.at(this->currentIndex);
}

// Kembalikan tipe token saat ini (tanpa value)
// Contoh: "ident(Hello)" → "ident"
std::string TokenStream::currentType() const{
    std::string curr = this->current();
    std::string type = "";

    int i = 0;
    while (i < (int)curr.size() && curr[i] != '('){
        type.push_back(curr[i]);
        i++;
    }

    return type;
}

// Kembalikan value token saat ini
// Contoh: "ident(Hello)" → "Hello"
std::string TokenStream::currentValue() const{
    std::string curr = this->current();
    int i = 0;
    while(i < (int)curr.size() && curr[i] != '('){
        i++;
    }

    if(i >= (int)curr.size()) return "";

    std::string value = "";
    int j = i + 1;
    while(j < (int)curr.size() && curr[j] != ')'){
        value.push_back(curr[j]);
        j++;
    }

    return value;
}

// Maju ke token berikutnya (seperti ADV() di lexer)
void TokenStream::advance(){
    if(this->currentIndex + 1 < (int)this->tokens.size()){
        this->currentIndex++;
    }
}

// Cek apakah sudah habis
bool TokenStream::isEOF() const{
    return this->currentIndex >= (int)tokens.size();
}

// Cek apakah token saat ini bertipe tertentu
bool TokenStream::check(const std::string& type) const{
    return this->currentType() == type;
}

// Peek token berikutnya tanpa maju
std::string TokenStream::peek() const{
    if(this->currentIndex + 1 < (int)this->tokens.size()) {
        return this->tokens.at(this->currentIndex + 1);
    }

    return "";
}

std::string TokenStream::peekType() const{
    std::string curr = this->peek();
    std::string type = "";

    int i = 0;
    while (i < (int)curr.size() && curr[i] != '('){
        type.push_back(curr[i]);
        i++;
    }

    return type;
}

// Posisi saat ini (berguna untuk error message)
int TokenStream::getIndex() const{
    return this->currentIndex;
}

std::string TokenStream::expect(const std::string& type, const std::string& context){
    if (!check(type)) {
        // lempar error — akan diimplementasikan setelah error.hpp selesai
        throw std::runtime_error(
            "Syntax error: unexpected '" + current() + "', expected '" + type + "'" + (context.empty() ? "" : " in " + context));
    }
    std::string tok = current();
    advance();
    return tok; 
}
