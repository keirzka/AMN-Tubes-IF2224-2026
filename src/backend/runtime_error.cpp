#include "runtime_error.hpp"
#include <iostream>
#include <sstream>

std::string runtimeErrorTypeToString(RuntimeErrorType type) {
    switch (type) {
        case RuntimeErrorType::DIVISION_BY_ZERO:   return "DIVISION_BY_ZERO";
        case RuntimeErrorType::TYPE_MISMATCH:      return "TYPE_MISMATCH";
        case RuntimeErrorType::UNDEFINED_VARIABLE: return "UNDEFINED_VARIABLE";
        case RuntimeErrorType::STACK_OVERFLOW:     return "STACK_OVERFLOW";
        case RuntimeErrorType::STACK_UNDERFLOW:    return "STACK_UNDERFLOW";
        case RuntimeErrorType::STACK_SMASHING:     return "STACK_SMASHING";
        case RuntimeErrorType::STACK_CORRUPTION:   return "STACK_CORRUPTION";
        case RuntimeErrorType::OOB_ACCESS:         return "OOB_ACCESS";
        case RuntimeErrorType::INVALID_JUMP:       return "INVALID_JUMP";
        case RuntimeErrorType::NUMERIC_OVERFLOW:   return "NUMERIC_OVERFLOW";
        case RuntimeErrorType::NUMERIC_UNDERFLOW:  return "NUMERIC_UNDERFLOW";
        default:                                   return "UNKNOWN_ERROR";
    }
}

RuntimeError::RuntimeError(RuntimeErrorType type,
                           const std::string& message,
                           int pc)
    : type(type), message(message), pc(pc) {}

std::string RuntimeError::toString() const {
    std::ostringstream oss;
    oss << "[RUNTIME ERROR] " << runtimeErrorTypeToString(type);
    if (pc >= 0) {
        oss << " at PC=" << pc;
    }
    oss << ": " << message;
    return oss.str();
}

ArionRuntimeException::ArionRuntimeException(RuntimeErrorType type,
                                             const std::string& message,
                                             int pc)
    : std::runtime_error(message), errorType(type), pc(pc) {}

void RuntimeErrorCollector::add(RuntimeErrorType type,
                                const std::string& message,
                                int pc) {
    errors.emplace_back(type, message, pc);
}

bool RuntimeErrorCollector::hasErrors() const {
    return !errors.empty();
}

int RuntimeErrorCollector::count() const {
    return static_cast<int>(errors.size());
}

void RuntimeErrorCollector::printAll() const {
    for (const RuntimeError& e : errors) {
        std::cerr << e.toString() << std::endl;
    }
}

void RuntimeErrorCollector::throwFirst() const {
    if (!errors.empty()) {
        const RuntimeError& e = errors.front();
        throw ArionRuntimeException(e.type, e.message, e.pc);
    }
}

const RuntimeError& RuntimeErrorCollector::first() const {
    return errors.front();
}

void RuntimeErrorCollector::clear() {
    errors.clear();
}