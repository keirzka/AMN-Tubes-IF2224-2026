#include "instruction.hpp"
#include <sstream>
#include <stdexcept>

Instruction::Instruction(int line, Mnemonic mnemonic, int level, int operand)
    : line(line), mnemonic(mnemonic), level(level), operand(operand) {}

std::string Instruction::toString() const {
    std::ostringstream oss;
    oss << line << " " << mnemonicToString(mnemonic)
        << " " << level
        << " " << operand;
    return oss.str();
}

std::string mnemonicToString(Mnemonic m) {
    switch (m) {
        case Mnemonic::LIT: return "LIT";
        case Mnemonic::LOD: return "LOD";
        case Mnemonic::STO: return "STO";
        case Mnemonic::CAL: return "CAL";
        case Mnemonic::INT: return "INT";
        case Mnemonic::JMP: return "JMP";
        case Mnemonic::JPC: return "JPC";
        case Mnemonic::OPR: return "OPR";
        case Mnemonic::RET: return "RET";
        default:            return "UNKNOWN";
    }
}

std::string oprCodeToString(OprCode o) {
    switch (o) {
        case OprCode::NEG:   return "NEG";
        case OprCode::ADD:   return "ADD";
        case OprCode::SUB:   return "SUB";
        case OprCode::MUL:   return "MUL";
        case OprCode::DIV:   return "DIV";
        case OprCode::MOD:   return "MOD";
        case OprCode::EQL:   return "EQL";
        case OprCode::NEQ:   return "NEQ";
        case OprCode::LSS:   return "LSS";
        case OprCode::GEQ:   return "GEQ";
        case OprCode::GTR:   return "GTR";
        case OprCode::LEQ:   return "LEQ";
        case OprCode::WRT:   return "WRT";
        case OprCode::WRTLN: return "WRTLN";
        default:             return "UNKNOWN_OPR";
    }
}

OprCode intToOprCode(int val) {
    switch (val) {
        case 1:  return OprCode::NEG;
        case 2:  return OprCode::ADD;
        case 3:  return OprCode::SUB;
        case 4:  return OprCode::MUL;
        case 5:  return OprCode::DIV;
        case 6:  return OprCode::MOD;
        case 7:  return OprCode::EQL;
        case 8:  return OprCode::NEQ;
        case 9:  return OprCode::LSS;
        case 10: return OprCode::GEQ;
        case 11: return OprCode::GTR;
        case 12: return OprCode::LEQ;
        case 13: return OprCode::WRT;
        case 14: return OprCode::WRTLN;
        default:
            throw std::invalid_argument(
                "intToOprCode: nilai " + std::to_string(val) +
                " bukan OprCode yang valid (harus 1-14)");
    }
}