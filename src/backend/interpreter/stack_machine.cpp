#include "backend/interpreter/stack_machine.hpp"
#include <sstream>

StackMachine::StackMachine(RuntimeErrorCollector& errorCollector)
    : SP(-1), BP(0), frameDepth(0), errors(errorCollector)
{
    stack.reserve(256);
}

void StackMachine::initMainFrame() {
    stack.clear();
    SP = -1;
    BP = 0;
    frameDepth = 0;

    push(RuntimeValue::fromInt(0));
    push(RuntimeValue::fromInt(0));
    push(RuntimeValue::fromInt(0));

    BP = 0;
    frameDepth = 1;
}

void StackMachine::push(const RuntimeValue& val) {
    if (SP + 1 >= MAX_STACK_SIZE) {
        errors.add(RuntimeErrorType::STACK_OVERFLOW,
                   "Stack overflow: ukuran stack melebihi batas " +
                   std::to_string(MAX_STACK_SIZE));
        errors.throwFirst();
    }

    if (SP + 1 < (int)stack.size()) {
        stack[SP + 1] = val;
    } else {
        stack.push_back(val);
    }
    ++SP;
}

RuntimeValue StackMachine::pop() {
    if (SP < 0) {
        errors.add(RuntimeErrorType::STACK_UNDERFLOW,
                   "Stack underflow: pop dari stack kosong");
        errors.throwFirst();
    }

    RuntimeValue val = stack[SP];
    --SP;
    return val;
}

RuntimeValue& StackMachine::peek() {
    if (SP < 0) {
        errors.add(RuntimeErrorType::STACK_UNDERFLOW,
                   "Stack underflow: peek dari stack kosong");
        errors.throwFirst();
    }
    return stack[SP];
}

RuntimeValue StackMachine::readMem(int address) {
    if (address < 0 || address > SP) {
        errors.add(RuntimeErrorType::OOB_ACCESS,
                   "readMem: address " + std::to_string(address) +
                   " di luar batas stack yang valid [0.." + std::to_string(SP) + "]");
        errors.throwFirst();
    }

    const RuntimeValue& val = stack[address];
    if (val.isNil()) {
        errors.add(RuntimeErrorType::UNDEFINED_VARIABLE,
                   "readMem: membaca variabel yang belum diinisialisasi di address " +
                   std::to_string(address));
        errors.throwFirst();
    }

    return val;
}

void StackMachine::writeMem(int address, const RuntimeValue& val) {
    if (address < 0 || address > SP) {
        errors.add(RuntimeErrorType::OOB_ACCESS,
                   "writeMem: address " + std::to_string(address) +
                   " di luar batas stack yang valid [0.." + std::to_string(SP) + "]");
        errors.throwFirst();
    }

    if (frameDepth > 1) {
        int checkBP = stack[BP + FRAME_DYNAMIC_LINK].ival;
        while (checkBP > 0) {
            if (checkBP + FRAME_RETURN_ADDR > SP) break;

            if (address >= checkBP &&
                address <= checkBP + FRAME_RETURN_ADDR) {
                errors.add(RuntimeErrorType::STACK_SMASHING,
                           "writeMem: penulisan ke address " +
                           std::to_string(address) +
                           " menimpa header frame pemanggil (Stack Smashing)");
                errors.throwFirst();
            }

            if (checkBP + FRAME_DYNAMIC_LINK > SP) break;
            int prevBP = stack[checkBP + FRAME_DYNAMIC_LINK].ival;
            if (!stack[checkBP + FRAME_DYNAMIC_LINK].isInt()) break;
            if (prevBP < 0 || prevBP >= checkBP) break;
            checkBP = prevBP;
        }
    }

    stack[address] = val;
}

RuntimeValue StackMachine::readMemAtFrame(int frameBase, int offset) {
    int address = frameBase + offset;
    if (address < 0 || address > SP) {
        errors.add(RuntimeErrorType::OOB_ACCESS,
                   "readMemAtFrame: address " + std::to_string(address) +
                   " (frameBase=" + std::to_string(frameBase) +
                   ", offset=" + std::to_string(offset) +
                   ") di luar batas stack [0.." + std::to_string(SP) + "]");
        errors.throwFirst();
    }

    const RuntimeValue& val = stack[address];
    if (val.isNil()) {
        errors.add(RuntimeErrorType::UNDEFINED_VARIABLE,
                   "readMemAtFrame: membaca variabel yang belum diinisialisasi di address " +
                   std::to_string(address) +
                   " (frameBase=" + std::to_string(frameBase) +
                   ", offset=" + std::to_string(offset) + ")");
        errors.throwFirst();
    }

    return val;
}

void StackMachine::writeMemAtFrame(int frameBase, int offset, const RuntimeValue& val) {
    int address = frameBase + offset;
    if (address < 0 || address > SP) {
        errors.add(RuntimeErrorType::OOB_ACCESS,
                   "writeMemAtFrame: address " + std::to_string(address) +
                   " (frameBase=" + std::to_string(frameBase) +
                   ", offset=" + std::to_string(offset) +
                   ") di luar batas stack [0.." + std::to_string(SP) + "]");
        errors.throwFirst();
    }
    stack[address] = val;
}

void StackMachine::enterFrame(int staticLink, int dynamicLink, int returnAddr) {
    if (frameDepth >= MAX_FRAME_DEPTH) {
        errors.add(RuntimeErrorType::STACK_OVERFLOW,
                   "Stack overflow: kedalaman frame melebihi batas " +
                   std::to_string(MAX_FRAME_DEPTH) +
                   " (kemungkinan infinite recursion)");
        errors.throwFirst();
    }

    int newBP = SP + 1;

    push(RuntimeValue::fromInt(staticLink));
    push(RuntimeValue::fromInt(dynamicLink));
    push(RuntimeValue::fromInt(returnAddr));

    BP = newBP;
    ++frameDepth;
}

int StackMachine::exitFrame() {
    if (BP < 0 || BP > SP) {
        errors.add(RuntimeErrorType::STACK_CORRUPTION,
                   "exitFrame: BP=" + std::to_string(BP) +
                   " tidak valid (SP=" + std::to_string(SP) + ")");
        errors.throwFirst();
    }

    if (BP + FRAME_RETURN_ADDR > SP) {
        errors.add(RuntimeErrorType::STACK_CORRUPTION,
                   "exitFrame: frame header tidak lengkap, BP=" +
                   std::to_string(BP) + " SP=" + std::to_string(SP));
        errors.throwFirst();
    }

    if (!stack[BP + FRAME_RETURN_ADDR].isInt() ||
        !stack[BP + FRAME_DYNAMIC_LINK].isInt()) {
        errors.add(RuntimeErrorType::STACK_CORRUPTION,
                   "exitFrame: header frame rusak (bukan INTEGER) di BP=" +
                   std::to_string(BP));
        errors.throwFirst();
    }

    int returnAddr  = stack[BP + FRAME_RETURN_ADDR].ival;
    int dynamicLink = stack[BP + FRAME_DYNAMIC_LINK].ival;

    SP = BP - 1;
    BP = dynamicLink;

    --frameDepth;
    return returnAddr;
}

void StackMachine::allocFrame(int frameSize) {
    int varsToAlloc = frameSize - FRAME_VAR_START;

    if (varsToAlloc < 0) {
        varsToAlloc = 0;
    }

    if (SP + varsToAlloc >= MAX_STACK_SIZE) {
        errors.add(RuntimeErrorType::STACK_OVERFLOW,
                   "allocFrame: alokasi " + std::to_string(frameSize) +
                   " slot melebihi batas stack " + std::to_string(MAX_STACK_SIZE));
        errors.throwFirst();
    }

    for (int i = 0; i < varsToAlloc; ++i) {
        push(RuntimeValue());
    }
}

int StackMachine::findFrameBase(int levelDiff) {
    if (levelDiff == 0) return BP;

    int base = BP;
    for (int i = 0; i < levelDiff; ++i) {
        if (base < 0 || base + FRAME_STATIC_LINK > SP) {
            errors.add(RuntimeErrorType::STACK_CORRUPTION,
                       "findFrameBase: static link chain rusak pada level " +
                       std::to_string(i) +
                       " (base=" + std::to_string(base) + ")");
            errors.throwFirst();
        }

        if (!stack[base + FRAME_STATIC_LINK].isInt()) {
            errors.add(RuntimeErrorType::STACK_CORRUPTION,
                       "findFrameBase: static link bukan INTEGER pada level " +
                       std::to_string(i));
            errors.throwFirst();
        }

        base = stack[base + FRAME_STATIC_LINK].ival;
    }
    return base;
}

int StackMachine::getSP()         const { return SP; }
int StackMachine::getBP()         const { return BP; }
int StackMachine::getFrameDepth() const { return frameDepth; }
int StackMachine::getStackSize()  const { return SP + 1; }

bool StackMachine::checkCorruption() {
    if (frameDepth == 0) return BP == 0 && SP == -1;
    if (BP < 0)          return false;

    int current = BP;
    int count   = 0;

    while (current > 0) {
        if (current + FRAME_RETURN_ADDR > SP) return false;

        if (!stack[current + FRAME_DYNAMIC_LINK].isInt()) return false;

        int dl = stack[current + FRAME_DYNAMIC_LINK].ival;

        if (dl < 0)       return false;
        if (dl >= current) return false;

        current = dl;
        ++count;
        if (count > MAX_FRAME_DEPTH) return false;
    }

    return true;
}