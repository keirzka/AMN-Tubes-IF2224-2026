#ifndef __TOKEN_STREAM_HPP_
#define __TOKEN_STREAM_HPP_

#pragma once

#include <string>
#include <vector>

class TokenStream {
private:
    std::vector<std::string> tokens;  // list token dari lexer
    int currentIndex;                  // posisi token saat ini

public:
    // Inisialisasi dengan token list dari lexer
    TokenStream(const std::vector<std::string>& tokenList);

    // Kembalikan token saat ini (tanpa maju)
    std::string current() const;

    // Kembalikan tipe token saat ini (tanpa value)
    // Contoh: "ident(Hello)" → "ident"
    std::string currentType() const;

    // Kembalikan value token saat ini
    // Contoh: "ident(Hello)" → "Hello"
    std::string currentValue() const;

    // Maju ke token berikutnya (seperti ADV() di lexer)
    void advance();

    // Cek apakah sudah habis
    bool isEOF() const;

    // Cek apakah token saat ini bertipe tertentu
    bool check(const std::string& type) const;

    // Peek token berikutnya tanpa maju
    std::string peek() const;
    std::string peekType() const;

    // Posisi saat ini (berguna untuk error message)
    int getIndex() const;

    std::string expect(const std::string& type, const std::string& context = "");
};

#endif