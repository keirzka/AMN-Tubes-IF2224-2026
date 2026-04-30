#include "parser/core/program_parser.hpp"
#include "parser/utils/error.hpp"

Node* parseProgram(TokenStream& ts){
    Node* node = new Node("<program>");

    // Parse program header
    node->addChild(parseProgramHeader(ts));

    // Parse declaration part 
    node->addChild(parseDeclarationPartCoordinator(ts));

    // Parse compound statement
    node->addChild(parseCompoundStatementCoordinator(ts));

    // Harus diakhiri dengan period
    if (!ts.check("period")) {
        throwSyntaxError(ts.current(), "period", ts.getIndex());
    }
    node->addChild(new Node(ts.current())); // terminal node "period"
    ts.advance();

    return node;
}

// <program-header> → programsy ident semicolon
Node* parseProgramHeader(TokenStream& ts) {
    Node* node = new Node("<program-header>");

    // Expect 'programsy'
    if (!ts.check("programsy")) {
        throwSyntaxError(ts.current(), "programsy", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();

    // Expect identifier (nama program)
    if (!ts.check("ident")) {
        throwSyntaxError(ts.current(), "ident", ts.getIndex());
    }
    node->addChild(new Node(ts.current())); // misal: ident(Hello)
    ts.advance();

    // Expect semicolon
    if (!ts.check("semicolon")) {
        throwSyntaxError(ts.current(), "semicolon", ts.getIndex());
    }
    node->addChild(new Node(ts.current()));
    ts.advance();

    return node;
}

// koordinasi declaration : const/type/var/subprogram
Node* parseDeclarationPartCoordinator(TokenStream& ts){
    Node* node = new Node("<declaration-part>");

    while (ts.check("constsy") || ts.check("typesy") || ts.check("varsy") || ts.check("proceduresy") || ts.check("functionsy")) {
        // Parse const declaration
        if(ts.check("constsy")) node->addChild(parseConstDeclaration(ts));

        // Parse type declaration
        else if(ts.check("typesy")) node->addChild(parseTypeDeclaration(ts));

        // Parse var declaration 
        else if(ts.check("varsy")) node->addChild(parseVarDeclaration(ts));

        // Parse subprogram declaration 
        else node->addChild(parseSubprogramDeclaration(ts));
    }

    return node;
}

// koordinasi statement : list/if/while/..
Node* parseCompoundStatementCoordinator(TokenStream& ts){
// TODO
    return nullptr;
}