#ifndef SEMANTIC_ERROR_HPP
#define SEMANTIC_ERROR_HPP

#include <string>
#include <vector>
#include <iostream>

// simpen satu pesan error dan konteksnya
struct SemanticError {
    std::string message;
    int line; // sumber, 0 kalau tidak tahu
    SemanticError(const std::string& msg, int ln = 0) : message(msg), line(ln) {}
};

// kumpulin semua error lalu cetak semuanya sekaligus
class SemanticErrorCollector {
private:
    std::vector<SemanticError> errors;
public:
    void add(const std::string& message, int line = 0);
    bool hasErrors() const;
    int count() const;
    void printAll() const;
    void clear();
};

#endif