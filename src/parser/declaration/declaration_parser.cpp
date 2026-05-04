#include "declaration_parser.hpp"
#include "parser/utils/error.hpp"
#include "parser/statement/statement_parser.hpp"

static std::string peekTypeTrimmed(TokenStream& ts) {
    std::string t = ts.peekType();
    while (!t.empty() && t.back() == ' ') {
        t.pop_back();
    }
    return t;
}

// cek apakah token saat ini bisa menjadi awal <constant>
Node* parseConstant(TokenStream& ts) {
    Node* node = new Node("<constant>");
    std::string t = ts.currentType();
    if (t == "charcon") {
        node->addChild(new Node(ts.current()));
        ts.advance();
        return node;
    }
    if (t == "string") {
        node->addChild(new Node(ts.current()));
        ts.advance();
        return node;
    }
    if (t == "plus" || t == "minus") {
        node->addChild(new Node(ts.current()));
        ts.advance();
        t = ts.currentType();
    }
    if (t == "ident" || t == "intcon" || t == "realcon") {
        node->addChild(new Node(ts.current()));
        ts.advance();
        return node;
    }
    throwSyntaxError(ts.current(), "constant (charcon | string | ident | intcon | realcon)", ts.getIndex());
    return nullptr;
}

Node* parseIdentifierList(TokenStream& ts) {
    Node* node = new Node("<identifier-list>");
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    while (ts.check("comma")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        if (!ts.check("ident")) {
            throwSyntaxError(ts.current(), "ident after comma", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    return node;
}

Node* parseRange(TokenStream& ts) {
    Node* node = new Node("<range>");
    node->addChild(parseConstant(ts));
    if (!ts.check("period")) {
        throwSyntaxError(ts.current(), "period (titik pertama range)", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("period")) {
        throwSyntaxError(ts.current(), "period (titik kedua range)", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseConstant(ts));
    return node;
}

Node* parseEnumerated(TokenStream& ts) {
    Node* node = new Node("<enumerated>");
    if (!ts.check("lparent")) {
        throwSyntaxError(ts.current(), "lparent", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident di dalam enumerated", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    while (ts.check("comma")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        if (!ts.check("ident")) {
            throwSyntaxError(ts.current(), "ident setelah comma di enumerated", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    if (!ts.check("rparent")) {
        throwSyntaxError(ts.current(), "rparent penutup enumerated", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    return node;
}

Node* parseArrayType(TokenStream& ts) {
    Node* node = new Node("<array-type>");
    if (!ts.check("arraysy")) {
        throwSyntaxError(ts.current(), "arraysy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("lbrack")) {
        throwSyntaxError(ts.current(), "lbrack", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    std::string t = ts.currentType();
    bool isRange = false;
    if (t == "plus" || t == "minus") {
        isRange = true;
    } else if (t == "intcon" || t == "realcon" || t == "charcon" || t == "string") {
        isRange = true;
    } else if (t == "ident") {
        isRange = (peekTypeTrimmed(ts) == "period");
    }
    if (isRange) {
        node->addChild(parseRange(ts));
    } else if (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    } else {
        throwSyntaxError(ts.current(), "range atau ident di dalam bracket array", ts.getIndex());
    }
    if (!ts.check("rbrack")) {
        throwSyntaxError(ts.current(), "rbrack", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ofsy")) {
        throwSyntaxError(ts.current(), "ofsy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseType(ts));
    return node;
}

Node* parseFieldPart(TokenStream& ts) {
    Node* node = new Node("<field-part>");
    node->addChild(parseIdentifierList(ts));
    if (!ts.check("colon")) {
        throwSyntaxError(ts.current(), "colon di field-part", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseType(ts));
    return node;
}

Node* parseFieldList(TokenStream& ts) {
    Node* node = new Node("<field-list>");
    node->addChild(parseFieldPart(ts));
    while (ts.check("semicolon")) {
        if (peekTypeTrimmed(ts) == "endsy") {
            break;
        }
        if (peekTypeTrimmed(ts) != "ident") {
            break;
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseFieldPart(ts));
    }
    return node;
}

Node* parseRecordType(TokenStream& ts) {
    Node* node = new Node("<record-type>");
    if (!ts.check("recordsy")) {
        throwSyntaxError(ts.current(), "recordsy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseFieldList(ts));
    if (!ts.check("endsy")) {
        throwSyntaxError(ts.current(), "endsy penutup record", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    return node;
}

Node* parseType(TokenStream& ts) {
    Node* node = new Node("<type>");
    std::string t = ts.currentType();
    if (t == "arraysy") {
        node->addChild(parseArrayType(ts));
        return node;
    }
    if (t == "lparent") {
        node->addChild(parseEnumerated(ts));
        return node;
    }
    if (t == "recordsy") {
        node->addChild(parseRecordType(ts));
        return node;
    }
    if (t == "plus" || t == "minus") {
        node->addChild(parseRange(ts));
        return node;
    }
    if (t == "intcon" || t == "realcon" || t == "charcon" || t == "string") {
        node->addChild(parseRange(ts));
        return node;
    }
    if (t == "ident") {
        if (peekTypeTrimmed(ts) == "period") {
            node->addChild(parseRange(ts));
        } else {
            node->addChild(new Node(ts.current()));
            ts.advance();
        }
        return node;
    }
    throwSyntaxError(ts.current(), "type (ident | array-type | range | enumerated | record-type)", ts.getIndex());
    return nullptr;
}

Node* parseConstDeclaration(TokenStream& ts) {
    Node* node = new Node("<const-declaration>");
    if (!ts.check("constsy")) {
        throwSyntaxError(ts.current(), "constsy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident (nama konstanta)", ts.getIndex());
    }
    while (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        if (!ts.check("eql")) {
            throwSyntaxError(ts.current(), "eql (==) setelah nama konstanta", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseConstant(ts));
        if (!ts.check("semicolon")) {
            throwSyntaxError(ts.current(), "semicolon setelah nilai konstanta", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    return node;
}

Node* parseTypeDeclaration(TokenStream& ts) {
    Node* node = new Node("<type-declaration>");
    if (!ts.check("typesy")) {
        throwSyntaxError(ts.current(), "typesy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident (nama type)", ts.getIndex());
    }
    while (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        if (!ts.check("eql")) {
            throwSyntaxError(ts.current(), "eql (==) setelah nama type", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseType(ts));
        if (!ts.check("semicolon")) {
            throwSyntaxError(ts.current(), "semicolon setelah definisi type", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    return node;
}

Node* parseVarDeclaration(TokenStream& ts) {
    Node* node = new Node("<var-declaration>");
    if (!ts.check("varsy")) {
        throwSyntaxError(ts.current(), "varsy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident (nama variabel)", ts.getIndex());
    }
    while (ts.check("ident")) {
        node->addChild(parseIdentifierList(ts));
        if (!ts.check("colon")) {
            throwSyntaxError(ts.current(), "colon setelah identifier-list", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseType(ts));
        if (!ts.check("semicolon")) {
            throwSyntaxError(ts.current(), "semicolon setelah deklarasi variabel", ts.getIndex());
        }
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    return node;
}

Node* parseParameterGroup(TokenStream& ts) {
    Node* node = new Node("<parameter-group>");
    node->addChild(parseIdentifierList(ts));
    if (!ts.check("colon")) {
        throwSyntaxError(ts.current(), "colon di parameter-group", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (ts.check("arraysy")) {
        node->addChild(parseArrayType(ts));
    } else if (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    } else {
        throwSyntaxError(ts.current(), "ident atau array-type sebagai tipe parameter", ts.getIndex());
    }
    return node;
}

Node* parseFormalParameterList(TokenStream& ts) {
    Node* node = new Node("<formal-parameter-list>");
    if (!ts.check("lparent")) {
        throwSyntaxError(ts.current(), "lparent", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseParameterGroup(ts));
    while (ts.check("semicolon")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseParameterGroup(ts));
    }
    if (!ts.check("rparent")) {
        throwSyntaxError(ts.current(), "rparent penutup formal-parameter-list", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    return node;
}

static Node* parseProcedureDeclaration(TokenStream& ts) {
    Node* node = new Node("<procedure-declaration>");
    if (!ts.check("proceduresy")) {
        throwSyntaxError(ts.current(), "proceduresy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident (nama prosedur)", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (ts.check("lparent")) {
        node->addChild(parseFormalParameterList(ts));
    }
    if (!ts.check("semicolon")) {
        throwSyntaxError(ts.current(), "semicolon setelah header prosedur", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseBlock(ts));
    if (!ts.check("semicolon")) {
        throwSyntaxError(ts.current(), "semicolon penutup prosedur", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    return node;
}

static Node* parseFunctionDeclaration(TokenStream& ts) {
    Node* node = new Node("<function-declaration>");
    if (!ts.check("functionsy")) {
        throwSyntaxError(ts.current(), "functionsy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident (nama fungsi)", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (ts.check("lparent")) {
        node->addChild(parseFormalParameterList(ts));
    }
    if (!ts.check("colon")) {
        throwSyntaxError(ts.current(), "colon sebelum return type", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident (return type)", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    if (!ts.check("semicolon")) {
        throwSyntaxError(ts.current(), "semicolon setelah header fungsi", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    node->addChild(parseBlock(ts));
    if (!ts.check("semicolon")) {
        throwSyntaxError(ts.current(), "semicolon penutup fungsi", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();
    return node;
}

Node* parseSubprogramDeclaration(TokenStream& ts) {
    Node* node = new Node("<subprogram-declaration>");
    if (ts.check("proceduresy")) {
        node->addChild(parseProcedureDeclaration(ts));
    } else if (ts.check("functionsy")) {
        node->addChild(parseFunctionDeclaration(ts));
    } else {
        throwSyntaxError(ts.current(), "proceduresy atau functionsy", ts.getIndex());
    }
    return node;
}

Node* parseBlock(TokenStream& ts) {
    Node* node = new Node("block");
    Node* declPart = new Node("<declaration-part>");
    bool hasDecl = false;
    while (ts.check("constsy") || ts.check("typesy") || ts.check("varsy") || ts.check("proceduresy") || ts.check("functionsy")) {
        hasDecl = true;
        if (ts.check("constsy")) declPart->addChild(parseConstDeclaration(ts));
        else if (ts.check("typesy")) declPart->addChild(parseTypeDeclaration(ts));
        else if (ts.check("varsy")) declPart->addChild(parseVarDeclaration(ts));
        else declPart->addChild(parseSubprogramDeclaration(ts));
    }
    if (hasDecl) {
        node->addChild(declPart);
    } else {
        delete declPart;
    }
    node->addChild(parseCompoundStatement(ts));
    return node;
}