#include "backend/codegen/address_resolver.hpp"
#include <stdexcept>
#include <vector>
#include <algorithm>

static int findBlockOfTab(const SymbolTable& st, int tabIndex) {
    for (int b = 0; b < (int)st.btab.size(); b++) {
        int k = st.btab[b].last;
        while (k != 0 && k < (int)st.tab.size()) {
            if (k == tabIndex) return b;
            k = st.tab[k].link;
        }
    }
    return -1;
}

static int computeVarOffset(const SymbolTable& st, int tabIndex) {
    int blockIdx = findBlockOfTab(st, tabIndex);
    if (blockIdx < 0) return 0;
    std::vector<int> varIndices;
    int k = st.btab[blockIdx].last;
    while (k != 0 && k < (int)st.tab.size()) {
        ObjClass oc = st.tab[k].obj;
        if (oc == ObjClass::VARIABLE || oc == ObjClass::FIELD) {
            varIndices.push_back(k);
        }
        k = st.tab[k].link;
    }
    std::reverse(varIndices.begin(), varIndices.end());
    for (int i = 0; i < (int)varIndices.size(); i++) {
        if (varIndices[i] == tabIndex) return i;
    }
    return 0;
}

int resolveVarAddress(const SymbolTable& st, int tabIndex) {
    if (tabIndex < 0 || tabIndex >= (int)st.tab.size()) return FRAME_HEADER_SIZE;
    return FRAME_HEADER_SIZE + computeVarOffset(st, tabIndex);
}

int resolveLevel(int currentLevel, int varLevel) {
    return currentLevel - varLevel;
}

int resolveFrameSize(const SymbolTable& st, int blkIdx) {
    if (blkIdx < 0 || blkIdx >= (int)st.btab.size()) return FRAME_HEADER_SIZE;
    return FRAME_HEADER_SIZE + st.btab[blkIdx].vsze;
}