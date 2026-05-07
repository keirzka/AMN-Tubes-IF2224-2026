// error.hpp
#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <stdexcept>

// Exception class untuk syntax error
class SyntaxError : public std::runtime_error {
public:
    SyntaxError(const std::string& message);
};

// Fungsi untuk melempar error dengan pesan informatif
// Contoh output: "Syntax error at token 5: unexpected 'period', expected 'semicolon'"
void throwSyntaxError(const std::string& unexpected, 
                      const std::string& expected,
                      int tokenIndex);

// Versi lebih fleksibel dengan pesan custom
void throwSyntaxError(const std::string& message, int tokenIndex);

#endif