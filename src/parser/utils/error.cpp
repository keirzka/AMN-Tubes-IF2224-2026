#include "error.hpp"
#include <sstream>

SyntaxError::SyntaxError(const std::string& message)
    : std::runtime_error(message) {}

void throwSyntaxError(const std::string& unexpected, const std::string& expected, int tokenIndex) {
    std::ostringstream oss;
    oss << "Syntax error at token " << tokenIndex << ": unexpected '" << unexpected << "', expected '" << expected << "'";
    throw SyntaxError(oss.str());
}

void throwSyntaxError(const std::string& message, int tokenIndex) {
    std::ostringstream oss;
    oss << "Syntax error at token " << tokenIndex << ": " << message;
    throw SyntaxError(oss.str());
}