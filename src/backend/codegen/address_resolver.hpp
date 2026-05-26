#ifndef ADDRESS_RESOLVER_HPP
#define ADDRESS_RESOLVER_HPP
#include "semantic/symbol_table.hpp"

static constexpr int FRAME_HEADER_SIZE = 3;

int resolveVarAddress(const SymbolTable& st, int tabIndex);
int resolveLevel(int currentLevel, int varLevel);
int resolveFrameSize(const SymbolTable& st, int blkIdx);

#endif