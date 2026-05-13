#include "semantic_error.hpp"

void SemanticErrorCollector::add(const std::string& message, int line) {
    errors.emplace_back(message, line);
}

bool SemanticErrorCollector::hasErrors() const {
    return !errors.empty();
}

int SemanticErrorCollector::count() const {
    return static_cast<int>(errors.size());
}

void SemanticErrorCollector::printAll() const {
    if (errors.empty()) return;
    for (const SemanticError& e : errors) {
        if (e.line > 0) {
            std::cerr << "Semantic error at line " << e.line << ": " << e.message << std::endl;
        } else {
            std::cerr << "Semantic error: " << e.message << std::endl;
        }
    }
}

void SemanticErrorCollector::clear() {
    errors.clear();
}