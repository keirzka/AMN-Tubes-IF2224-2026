#include "semantic/semantic_analyzer.hpp"
#include <algorithm>
#include <sstream>

std::string nodeTokenType(const Node* node) {
    const std::string& lbl = node->label;
    std::string type = "";
    int i = 0;
    while (i < (int)lbl.size() && lbl[i] != '(') {
        type.push_back(lbl[i]);
        i++;
    }
    while (!type.empty() && type.back() == ' ') type.pop_back();
    return type;
}

std::string nodeTokenValue(const Node* node) {
    const std::string& lbl = node->label;
    int i = 0;
    while (i < (int)lbl.size() && lbl[i] != '(') i++;
    if (i >= (int)lbl.size()) return "";
    std::string value = "";
    int j = i + 1;
    while (j < (int)lbl.size() && lbl[j] != ')') {
        value.push_back(lbl[j]);
        j++;
    }
    return value;
}

bool isNodeType(const Node* node, const std::string& label) {
    return node->label == label;
}

Node* findChild(Node* node, const std::string& label) {
    for (Node* c : node->children) {
        if (c->label == label) return c;
    }
    return nullptr;
}

Node* findChildByTokenType(Node* node, const std::string& tokenType) {
    for (Node* c : node->children) {
        if (nodeTokenType(c) == tokenType) return c;
    }
    return nullptr;
}

void analyze(Node* root, SemanticContext& ctx) {
    if (!root) return;
    visit_program(root, ctx);
}

static int parseIntValue(const std::string& s) {
    try { return std::stoi(s); } catch (...) { return 0; }
}

TypeInfo visit_constant_value(Node* node, SemanticContext& ctx) {
    if (!node) return TypeInfo(TypeCode::NOTYPE);

    if (node->label == "<constant>") {
        if (node->children.empty()) return TypeInfo(TypeCode::NOTYPE);

        bool negative = false;
        int ci = 0;
        std::string firstType = nodeTokenType(node->children[ci]);
        if (firstType == "plus") {
            ci++;
        } else if (firstType == "minus") {
            negative = true;
            ci++;
        }

        if (ci >= (int)node->children.size()) return TypeInfo(TypeCode::NOTYPE);

        Node* valNode = node->children[ci];
        std::string tt = nodeTokenType(valNode);
        std::string tv = nodeTokenValue(valNode);

        if (tt == "intcon") {
            int v = parseIntValue(tv);
            if (negative) v = -v;
            TypeInfo ti(TypeCode::INTEGER);
            ti.low = v; ti.high = v;
            return ti;
        }
        if (tt == "realcon") return TypeInfo(TypeCode::REAL);
        if (tt == "charcon") return TypeInfo(TypeCode::CHAR);
        if (tt == "string")  return TypeInfo(TypeCode::STRING);
        if (tt == "ident") {
            int idx = ctx.st.lookup(tv);
            if (idx < 0) {
                ctx.errors.add("Identifier tidak ditemukan: '" + tv + "'");
                return TypeInfo(TypeCode::NOTYPE);
            }
            TypeInfo ti = typeInfoFromTab(idx, ctx.st);
            valNode->annotate((int)ti.baseType, idx, ctx.st.currentLev());
            return ti;
        }
        return TypeInfo(TypeCode::NOTYPE);
    }

    std::string tt = nodeTokenType(node);
    std::string tv = nodeTokenValue(node);

    if (tt == "intcon") {
        TypeInfo ti(TypeCode::INTEGER);
        int v = parseIntValue(tv);
        ti.low = v; ti.high = v;
        return ti;
    }
    if (tt == "realcon") return TypeInfo(TypeCode::REAL);
    if (tt == "charcon") return TypeInfo(TypeCode::CHAR);
    if (tt == "string")  return TypeInfo(TypeCode::STRING);
    if (tt == "ident") {
        int idx = ctx.st.lookup(tv);
        if (idx < 0) {
            ctx.errors.add("Identifier tidak ditemukan: '" + tv + "'");
            return TypeInfo(TypeCode::NOTYPE);
        }
        TypeInfo ti = typeInfoFromTab(idx, ctx.st);
        node->annotate((int)ti.baseType, idx, ctx.st.currentLev());
        return ti;
    }
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo visit_range(Node* node, SemanticContext& ctx) {
    if (!node) return TypeInfo(TypeCode::NOTYPE);

    TypeInfo loTi, hiTi;
    int loVal = 0, hiVal = 0;

    std::vector<Node*> constants;
    for (Node* c : node->children) {
        if (c->label == "<constant>") constants.push_back(c);
    }

    bool negLo = false, negHi = false;

    auto extractConst = [&](Node* cn, bool& neg, TypeInfo& ti, int& val) {
        neg = false;
        if (cn->children.empty()) { ti = TypeInfo(TypeCode::NOTYPE); return; }
        int ci = 0;
        std::string ft = nodeTokenType(cn->children[ci]);
        if (ft == "plus")       { ci++; }
        else if (ft == "minus") { neg = true; ci++; }
        if (ci >= (int)cn->children.size()) { ti = TypeInfo(TypeCode::NOTYPE); return; }
        Node* v       = cn->children[ci];
        std::string ttype = nodeTokenType(v);
        std::string tval  = nodeTokenValue(v);
        if (ttype == "intcon") {
            val = parseIntValue(tval);
            if (neg) val = -val;
            ti = TypeInfo(TypeCode::INTEGER);
            ti.low = val; ti.high = val;
        } else if (ttype == "charcon") {
            val = (int)tval[0];
            ti  = TypeInfo(TypeCode::CHAR);
        } else if (ttype == "ident") {
            int idx = ctx.st.lookup(tval);
            if (idx < 0) {
                ctx.errors.add("Identifier tidak ditemukan di range: '" + tval + "'");
                ti = TypeInfo(TypeCode::NOTYPE); return;
            }
            ti  = typeInfoFromTab(idx, ctx.st);
            val = ctx.st.tab[idx].adr;
            v->annotate((int)ti.baseType, idx, ctx.st.currentLev());
        } else if (ttype == "realcon") {
            ctx.errors.add("Subrange tidak boleh bertipe Real");
            ti = TypeInfo(TypeCode::NOTYPE);
        } else {
            ti = TypeInfo(TypeCode::NOTYPE);
        }
    };

    if (constants.size() >= 2) {
        extractConst(constants[0], negLo, loTi, loVal);
        extractConst(constants[1], negHi, hiTi, hiVal);
    } else {
        ctx.errors.add("Range tidak lengkap");
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (loTi.baseType == TypeCode::NOTYPE || hiTi.baseType == TypeCode::NOTYPE)
        return TypeInfo(TypeCode::NOTYPE);

    if (loTi.baseType != hiTi.baseType) {
        ctx.errors.add("Tipe batas bawah dan atas range harus sama: " +
                       loTi.toString() + " vs " + hiTi.toString());
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (!isValidSubrangeType(loTi)) {
        ctx.errors.add("Subrange tidak boleh bertipe Real atau komposit");
        return TypeInfo(TypeCode::NOTYPE);
    }

    if (loVal > hiVal) {
        ctx.errors.add("Batas bawah range tidak boleh lebih besar dari batas atas: " +
                       std::to_string(loVal) + " > " + std::to_string(hiVal));
    }

    int atabIdx = ctx.st.insertArray(
        (int)loTi.baseType, (int)loTi.baseType,
        0, loVal, hiVal, 1
    );

    TypeInfo result(TypeCode::SUBRANGE, atabIdx, loVal, hiVal);
    result.subrangeBaseType = loTi.baseType;
    node->annotate((int)TypeCode::SUBRANGE, atabIdx, ctx.st.currentLev());
    return result;
}

TypeInfo visit_enumerated(Node* node, SemanticContext& ctx) {
    if (!node) return TypeInfo(TypeCode::NOTYPE);

    int ordinal = 0;
    for (Node* c : node->children) {
        if (nodeTokenType(c) == "ident") {
            std::string name = nodeTokenValue(c);
            if (ctx.st.lookupCurrentBlock(name) >= 0) {
                ctx.errors.add("Identifier enumerated sudah dideklarasikan: '" + name + "'");
            } else {
                int idx = ctx.st.insert(name, ObjClass::CONSTANT,
                                        TypeCode::ENUMERATED, 0, 1, ordinal);
                c->annotate((int)TypeCode::ENUMERATED, idx, ctx.st.currentLev());
                ordinal++;
            }
        }
    }

    node->annotate((int)TypeCode::ENUMERATED, 0, ctx.st.currentLev());
    return TypeInfo(TypeCode::ENUMERATED);
}

TypeInfo visit_array_type(Node* node, SemanticContext& ctx) {
    if (!node) return TypeInfo(TypeCode::NOTYPE);

    TypeInfo indexType(TypeCode::NOTYPE);
    TypeInfo elemType(TypeCode::NOTYPE);

    for (Node* c : node->children) {
        if (c->label == "<range>") {
            indexType = visit_range(c, ctx);
        } else if (c->label == "<type>") {
            elemType = visit_type(c, ctx);
        } else if (nodeTokenType(c) == "ident") {
            std::string tv = nodeTokenValue(c);
            int idx = ctx.st.lookup(tv);
            if (idx < 0) {
                ctx.errors.add("Tipe index array tidak ditemukan: '" + tv + "'");
            } else {
                indexType = typeInfoFromTab(idx, ctx.st);
                c->annotate((int)indexType.baseType, idx, ctx.st.currentLev());
            }
        }
    }

    if (!isValidIndexType(indexType)) {
        ctx.errors.add("Index array harus bertipe ordinal bukan Real: " + indexType.toString());
        indexType = TypeInfo(TypeCode::INTEGER);
    }

    int elemRef = 0;
    if (elemType.baseType == TypeCode::ARRAY || elemType.baseType == TypeCode::SUBRANGE) {
        elemRef = elemType.ref;
    }

    int atabIdx = ctx.st.insertArray(
        (int)indexType.baseType, (int)elemType.baseType,
        elemRef, indexType.low, indexType.high, 1
    );

    TypeInfo result(TypeCode::ARRAY, atabIdx, indexType.low, indexType.high);
    node->annotate((int)TypeCode::ARRAY, atabIdx, ctx.st.currentLev());
    return result;
}

TypeInfo visit_record_type(Node* node, SemanticContext& ctx, const std::string& namedId, int* btabIdxOut) {
    if (!node) return TypeInfo(TypeCode::NOTYPE);

    int btabIdx = ctx.st.enter_block();
    if (btabIdxOut) *btabIdxOut = btabIdx;

    for (Node* c : node->children) {
        if (c->label != "<field-list>") continue;
        for (Node* fp : c->children) {
            if (fp->label != "<field-part>") continue;

            TypeInfo fieldType(TypeCode::NOTYPE);
            std::vector<std::string> fieldNames;
            std::vector<Node*> fieldIdentNodes;

            for (Node* fc : fp->children) {
                if (fc->label == "<identifier-list>") {
                    for (Node* ic : fc->children) {
                        if (nodeTokenType(ic) == "ident") {
                            fieldNames.push_back(nodeTokenValue(ic));
                            fieldIdentNodes.push_back(ic);
                        }
                    }
                } else if (fc->label == "<type>") {
                    fieldType = visit_type(fc, ctx);
                }
            }

            for (int ni = 0; ni < (int)fieldNames.size(); ni++) {
                if (ctx.st.lookupCurrentBlock(fieldNames[ni]) >= 0) {
                    ctx.errors.add("Field record sudah dideklarasikan: '" + fieldNames[ni] + "'");
                } else {
                    int idx = ctx.st.insert(fieldNames[ni], ObjClass::FIELD,
                                            fieldType.baseType, fieldType.ref);
                    fieldIdentNodes[ni]->annotate((int)fieldType.baseType, idx, ctx.st.currentLev());
                }
            }
        }
    }

    ctx.st.exit_block();

    TypeInfo result(TypeCode::RECORD, btabIdx);
    result.isNamed = !namedId.empty();
    result.namedId = namedId;
    node->annotate((int)TypeCode::RECORD, btabIdx, ctx.st.currentLev());
    return result;
}

TypeInfo visit_type(Node* node, SemanticContext& ctx) {
    if (!node) return TypeInfo(TypeCode::NOTYPE);

    for (Node* c : node->children) {
        if (c->label == "<array-type>") {
            TypeInfo ti = visit_array_type(c, ctx);
            node->annotate((int)ti.baseType, ti.ref, ctx.st.currentLev());
            return ti;
        }
        if (c->label == "<record-type>") {
            int btabOut = 0;
            TypeInfo ti = visit_record_type(c, ctx, "", &btabOut);
            node->annotate((int)ti.baseType, btabOut, ctx.st.currentLev());
            return ti;
        }
        if (c->label == "<range>") {
            TypeInfo ti = visit_range(c, ctx);
            node->annotate((int)ti.baseType, ti.ref, ctx.st.currentLev());
            return ti;
        }
        if (c->label == "<enumerated>") {
            TypeInfo ti = visit_enumerated(c, ctx);
            node->annotate((int)ti.baseType, ti.ref, ctx.st.currentLev());
            return ti;
        }
        if (nodeTokenType(c) == "ident") {
            std::string tv = nodeTokenValue(c);
            int idx = ctx.st.lookup(tv);
            if (idx < 0) {
                ctx.errors.add("Tipe tidak ditemukan: '" + tv + "'");
                return TypeInfo(TypeCode::NOTYPE);
            }
            const TabEntry& e = ctx.st.tab[idx];
            if (e.obj != ObjClass::TYPE) {
                ctx.errors.add("'" + tv + "' bukan sebuah tipe");
                return TypeInfo(TypeCode::NOTYPE);
            }
            TypeInfo ti = typeInfoFromTab(idx, ctx.st);
            if (e.type == TypeCode::RECORD) {
                ti.isNamed = true;
                ti.namedId = tv;
            }
            c->annotate((int)ti.baseType, idx, ctx.st.currentLev());
            node->annotate((int)ti.baseType, idx, ctx.st.currentLev());
            return ti;
        }
    }

    return TypeInfo(TypeCode::NOTYPE);
}

void visit_const_declaration(Node* node, SemanticContext& ctx) {
    if (!node) return;

    int i = 0;
    int n = (int)node->children.size();

    if (i < n && nodeTokenType(node->children[i]) == "constsy") i++;

    while (i < n) {
        Node* identNode = node->children[i];
        if (nodeTokenType(identNode) != "ident") { i++; continue; }

        std::string name = nodeTokenValue(identNode);
        i++;

        if (i < n && nodeTokenType(node->children[i]) == "eql") i++;

        if (i >= n) break;

        TypeInfo ti = visit_constant_value(node->children[i], ctx);
        i++;

        if (i < n && nodeTokenType(node->children[i]) == "semicolon") i++;

        if (ctx.st.lookupCurrentBlock(name) >= 0) {
            ctx.errors.add("Identifier '" + name + "' sudah dideklarasikan dalam scope ini");
            continue;
        }

        int adr = (ti.baseType == TypeCode::INTEGER) ? ti.low : 0;
        int idx = ctx.st.insert(name, ObjClass::CONSTANT, ti.baseType, ti.ref, 1, adr);
        identNode->annotate((int)ti.baseType, idx, ctx.st.currentLev());
    }

    node->annotate((int)TypeCode::NOTYPE, -1, ctx.st.currentLev());
}

void visit_type_declaration(Node* node, SemanticContext& ctx) {
    if (!node) return;

    int i = 0;
    int n = (int)node->children.size();

    if (i < n && nodeTokenType(node->children[i]) == "typesy") i++;

    while (i < n) {
        Node* identNode = node->children[i];
        if (nodeTokenType(identNode) != "ident") { i++; continue; }

        std::string name = nodeTokenValue(identNode);
        i++;

        if (i < n && nodeTokenType(node->children[i]) == "eql") i++;

        if (i >= n) break;

        Node* typeNode = node->children[i];
        i++;

        if (i < n && nodeTokenType(node->children[i]) == "semicolon") i++;

        TypeInfo ti;
        if (typeNode->label == "<type>") {
            Node* inner = typeNode->children.empty() ? nullptr : typeNode->children[0];
            if (inner && inner->label == "<record-type>") {
                int btabOut = 0;
                ti = visit_record_type(inner, ctx, name, &btabOut);
                typeNode->annotate((int)ti.baseType, btabOut, ctx.st.currentLev());
            } else {
                ti = visit_type(typeNode, ctx);
                if (ti.baseType == TypeCode::RECORD) { ti.isNamed = true; ti.namedId = name; }
            }
        } else {
            ti = visit_type(typeNode, ctx);
        }

        if (ctx.st.lookupCurrentBlock(name) >= 0) {
            ctx.errors.add("Tipe '" + name + "' sudah dideklarasikan dalam scope ini");
            continue;
        }

        int idx = ctx.st.insert(name, ObjClass::TYPE, ti.baseType, ti.ref);
        identNode->annotate((int)ti.baseType, idx, ctx.st.currentLev());
    }
}

void visit_var_declaration(Node* node, SemanticContext& ctx) {
    if (!node) return;

    int i = 0;
    int n = (int)node->children.size();

    if (i < n && nodeTokenType(node->children[i]) == "varsy") i++;

    while (i < n) {
        Node* c = node->children[i];
        if (nodeTokenType(c) == "semicolon") { i++; continue; }
        if (c->label != "<identifier-list>") { i++; continue; }

        std::vector<std::string> names;
        std::vector<Node*> identNodes;
        for (Node* ic : c->children) {
            if (nodeTokenType(ic) == "ident") {
                names.push_back(nodeTokenValue(ic));
                identNodes.push_back(ic);
            }
        }
        i++;

        if (i < n && nodeTokenType(node->children[i]) == "colon") i++;

        if (i >= n) break;

        Node* typeNode = node->children[i];
        i++;

        if (i < n && nodeTokenType(node->children[i]) == "semicolon") i++;

        TypeInfo ti;
        if (typeNode->label == "<type>") {
            Node* inner = typeNode->children.empty() ? nullptr : typeNode->children[0];
            if (inner && inner->label == "<record-type>") {
                int btabOut = 0;
                ti = visit_record_type(inner, ctx, "", &btabOut);
                typeNode->annotate((int)ti.baseType, btabOut, ctx.st.currentLev());
            } else {
                ti = visit_type(typeNode, ctx);
            }
        } else {
            ti = visit_type(typeNode, ctx);
        }

        for (int ni = 0; ni < (int)names.size(); ni++) {
            if (ctx.st.lookupCurrentBlock(names[ni]) >= 0) {
                ctx.errors.add("Variabel '" + names[ni] + "' sudah dideklarasikan dalam scope ini");
            } else {
                int idx = ctx.st.insert(names[ni], ObjClass::VARIABLE, ti.baseType, ti.ref);
                identNodes[ni]->annotate((int)ti.baseType, idx, ctx.st.currentLev());
            }
        }
    }
}

void visit_formal_parameter_list(Node* node, SemanticContext& ctx, int btabIdx) {
    if (!node) return;
    int paramCount = 0;
    for (Node* c : node->children) {
        if (c->label != "<parameter-group>") continue;
        TypeInfo paramType(TypeCode::NOTYPE);
        std::vector<std::string> paramNames;
        std::vector<Node*> paramIdentNodes;
        for (Node* pc : c->children) {
            if (pc->label == "<identifier-list>") {
                for (Node* ic : pc->children) {
                    if (nodeTokenType(ic) == "ident") {
                        paramNames.push_back(nodeTokenValue(ic));
                        paramIdentNodes.push_back(ic);
                    }
                }
            } else if (pc->label == "<array-type>") {
                paramType = visit_array_type(pc, ctx);
            } else if (pc->label == "<type>") {
                paramType = visit_type(pc, ctx);
            } else if (nodeTokenType(pc) == "ident") {
                std::string tv = nodeTokenValue(pc);
                int idx = ctx.st.lookup(tv);
                if (idx < 0) {
                    ctx.errors.add("Tipe parameter tidak ditemukan: '" + tv + "'");
                } else {
                    paramType = typeInfoFromTab(idx, ctx.st);
                    pc->annotate((int)paramType.baseType, idx, ctx.st.currentLev());
                }
            }
        }
        for (int ni = 0; ni < (int)paramNames.size(); ni++) {
            if (ctx.st.lookupCurrentBlock(paramNames[ni]) >= 0) {
                ctx.errors.add("Parameter '" + paramNames[ni] + "' sudah dideklarasikan");
            } else {
                int idx = ctx.st.insert(paramNames[ni], ObjClass::VARIABLE,
                                        paramType.baseType, paramType.ref, 1, paramCount);
                paramIdentNodes[ni]->annotate((int)paramType.baseType, idx, ctx.st.currentLev());
                paramCount++;
            }
        }
    }
    ctx.st.btab[btabIdx].lpar = ctx.st.btab[btabIdx].last;
    ctx.st.btab[btabIdx].psze = paramCount;
}

void visit_procedure_declaration(Node* node, SemanticContext& ctx) {
    if (!node) return;
    std::string procName = "";
    Node* fplNode = nullptr, *blockNode = nullptr;
    bool foundFirst = false;
    for (Node* c : node->children) {
        if (nodeTokenType(c) == "ident" && !foundFirst) { procName = nodeTokenValue(c); foundFirst = true; }
        else if (c->label == "<formal-parameter-list>") fplNode = c;
        else if (c->label == "block") blockNode = c;
    }
    if (procName.empty()) return;
    if (ctx.st.lookupCurrentBlock(procName) >= 0)
        ctx.errors.add("Prosedur '" + procName + "' sudah dideklarasikan dalam scope ini");
    int procIdx = ctx.st.insert(procName, ObjClass::PROCEDURE, TypeCode::NOTYPE);
    int btabIdx = ctx.st.enter_block();
    ctx.st.tab[procIdx].ref = btabIdx;
    if (fplNode)   visit_formal_parameter_list(fplNode, ctx, btabIdx);
    if (blockNode) visit_block(blockNode, ctx);
    ctx.st.exit_block();
    Node* identNode = findChildByTokenType(node, "ident");
    if (identNode) identNode->annotate((int)TypeCode::NOTYPE, procIdx, ctx.st.currentLev());
}

void visit_function_declaration(Node* node, SemanticContext& ctx) {
    if (!node) return;
    std::string funcName = "", retTypeName = "";
    Node* fplNode = nullptr, *blockNode = nullptr, *retTypeNode = nullptr;
    bool foundFirst = false;
    for (Node* c : node->children) {
        if (nodeTokenType(c) == "ident") {
            if (!foundFirst) { funcName = nodeTokenValue(c); foundFirst = true; }
            else { retTypeName = nodeTokenValue(c); retTypeNode = c; }
        } else if (c->label == "<formal-parameter-list>") fplNode = c;
        else if (c->label == "block") blockNode = c;
    }
    if (funcName.empty()) return;
    TypeInfo retType(TypeCode::NOTYPE);
    if (!retTypeName.empty()) {
        int rtIdx = ctx.st.lookup(retTypeName);
        if (rtIdx < 0) {
            ctx.errors.add("Return type fungsi tidak ditemukan: '" + retTypeName + "'");
        } else {
            retType = typeInfoFromTab(rtIdx, ctx.st);
            if (retTypeNode) retTypeNode->annotate((int)retType.baseType, rtIdx, ctx.st.currentLev());
        }
    }
    if (ctx.st.lookupCurrentBlock(funcName) >= 0)
        ctx.errors.add("Fungsi '" + funcName + "' sudah dideklarasikan dalam scope ini");
    int funcIdx = ctx.st.insert(funcName, ObjClass::FUNCTION, retType.baseType, retType.ref);
    int btabIdx = ctx.st.enter_block();
    ctx.st.tab[funcIdx].ref = btabIdx;
    if (fplNode)   visit_formal_parameter_list(fplNode, ctx, btabIdx);
    if (blockNode) visit_block(blockNode, ctx);
    ctx.st.exit_block();
    Node* identNode = findChildByTokenType(node, "ident");
    if (identNode) identNode->annotate((int)retType.baseType, funcIdx, ctx.st.currentLev());
}

void visit_subprogram_declaration(Node* node, SemanticContext& ctx) {
    if (!node) return;
    for (Node* c : node->children) {
        if (c->label == "<procedure-declaration>")     visit_procedure_declaration(c, ctx);
        else if (c->label == "<function-declaration>") visit_function_declaration(c, ctx);
    }
}

void visit_declaration_part(Node* node, SemanticContext& ctx) {
    if (!node) return;
    for (Node* c : node->children) {
        if (c->label == "<const-declaration>")           visit_const_declaration(c, ctx);
        else if (c->label == "<type-declaration>")       visit_type_declaration(c, ctx);
        else if (c->label == "<var-declaration>")        visit_var_declaration(c, ctx);
        else if (c->label == "<subprogram-declaration>") visit_subprogram_declaration(c, ctx);
    }
}

void visit_block(Node* node, SemanticContext& ctx) {
    if (!node) return;
    for (Node* c : node->children) {
        if (c->label == "<declaration-part>")        visit_declaration_part(c, ctx);
        else if (c->label == "<compound-statement>") visit_compound_statement(c, ctx);
    }
}

void visit_program_header(Node* node, SemanticContext& ctx) {
    if (!node) return;
    for (Node* c : node->children) {
        if (nodeTokenType(c) == "ident") {
            std::string name = nodeTokenValue(c);
            int idx = ctx.st.insert(name, ObjClass::PROGRAM, TypeCode::NOTYPE);
            c->annotate((int)TypeCode::NOTYPE, idx, ctx.st.currentLev());
            break;
        }
    }
    node->annotate((int)TypeCode::NOTYPE, -1, ctx.st.currentLev());
}

void visit_program(Node* node, SemanticContext& ctx) {
    if (!node) return;
    for (Node* c : node->children) {
        if (c->label == "<program-header>")          visit_program_header(c, ctx);
        else if (c->label == "<declaration-part>")   visit_declaration_part(c, ctx);
        else if (c->label == "<compound-statement>") visit_compound_statement(c, ctx);
    }
    node->annotate((int)TypeCode::NOTYPE, -1, 0);
}

TypeInfo visit_expression(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo visit_simple_expression(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo visit_term(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo visit_factor(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo visit_variable(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
    return TypeInfo(TypeCode::NOTYPE);
}

TypeInfo visit_component_variable(Node* node, SemanticContext& ctx, TypeInfo baseType) {
    (void)node; (void)ctx;
    return baseType;
}

void visit_procedure_function_call(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_compound_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_statement_list(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_assignment_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_if_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_while_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_repeat_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_for_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_case_statement(Node* node, SemanticContext& ctx) {
    (void)node; (void)ctx;
}

void visit_case_list(Node* node, SemanticContext& ctx, const TypeInfo& exprType) {
    (void)node; (void)ctx; (void)exprType;
}

void visit_case_branch(Node* node, SemanticContext& ctx, const TypeInfo& exprType) {
    (void)node; (void)ctx; (void)exprType;
}