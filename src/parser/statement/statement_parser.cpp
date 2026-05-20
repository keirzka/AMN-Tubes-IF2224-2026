#include "statement_parser.hpp"
#include "parser/utils/error.hpp"
#include "parser/expression/expression_parser.hpp"

Node* parseCompoundStatement(TokenStream& ts){
    Node* node = new Node("<compound-statement>");

    node->addChild(new Node(ts.expect("beginsy", "compound statement")));

    node->addChild(parseStatementList(ts));

    node->addChild(new Node(ts.expect("endsy", "compound statement")));

    return node;
}

Node* parseStatementList(TokenStream& ts){
    Node* node = new Node("<statement-list>");

    node->addChild(parseStatement(ts));

    while (ts.check("semicolon")) {
        node->addChild(new Node(ts.current()));
        ts.advance();

        if (ts.check("endsy") || ts.check("untilsy")) break;

        node->addChild(parseStatement(ts));
    }

    return node;
}

Node* parseStatement(TokenStream& ts){
    Node* node = new Node("<statement>");

    // node->addChild

    if (ts.check("ident")) {
        int saveIndex = ts.getIndex();
        try {
            node->addChild (parseAssignmentStatement(ts));
            // return parseAssignmentStatement(ts);
            return node;
        } catch (...) {
            ts.setIndex(saveIndex);
            node->addChild (parseProcedureFunctionCall(ts));
            return node;
        }
    }
    else if (ts.check("ifsy")) { node->addChild(parseIfStatement(ts)); return node; }
    else if (ts.check("whilesy")) {node->addChild(parseWhileStatement(ts)); return node; }
    else if (ts.check("repeatsy")) {node->addChild(parseRepeatStatement(ts)); return node;}
    else if (ts.check("forsy")) {node->addChild(parseForStatement(ts)); return node;}
    else if (ts.check("casesy")) {node->addChild(parseCaseStatement(ts)); return node;}
    else if (ts.check("beginsy")) {node->addChild(parseCompoundStatement(ts)); return node;}

    if (ts.isEOF() || ts.check("endsy") || ts.check("untilsy") || ts.check("elsesy")) {
        return new Node("<statement>");
    }

    // Node* node = new Node("<unknown-statement>");
    node->addChild(new Node(ts.current()));
    ts.advance();
    return node;
}

Node* parseAssignmentStatement(TokenStream& ts){
    Node* node = new Node("<assignment-statement>");

    // node->addChild(new Node(ts.expect("ident", "assignment")));
    node->addChild (parseVariable(ts));
    node->addChild(new Node(ts.expect("becomes", "assignment")));

    node->addChild(parseExpression(ts)); 

    return node;
}

Node* parseProcedureFunctionCall(TokenStream& ts){
    Node* node = new Node("<procedure/function-call>");

    node->addChild(new Node(ts.expect("ident", "procedure call")));

    if (ts.check("lparent")) {
        node->addChild(new Node(ts.current()));
        ts.advance();

        if (!ts.check("rparent")) {
            // node->addChild(parseExpressionList(ts));
            node->addChild (parseParameterList(ts));
        }

        node->addChild(new Node(ts.expect("rparent", "procedure call")));
    }

    while (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    }

    return node;
}

Node* parseIfStatement(TokenStream& ts){
    Node* node = new Node("<if-statement>");

    node->addChild(new Node(ts.expect("ifsy")));
    node->addChild(parseExpression(ts));
    node->addChild(new Node(ts.expect("thensy")));

    node->addChild(parseStatement(ts));

    if (ts.check("elsesy")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseStatement(ts));
    }

    return node;
}

Node* parseWhileStatement(TokenStream& ts){
    Node* node = new Node("<while-statement>");

    node->addChild(new Node(ts.expect("whilesy")));
    node->addChild(parseExpression(ts));
    node->addChild(new Node(ts.expect("dosy")));

    // node->addChild(parseStatement(ts));
    node->addChild (parseCompoundStatement(ts));
    // node->addChild(new Node(ts.expect("semicolon")));

    return node;
}

Node* parseRepeatStatement(TokenStream& ts){
    Node* node = new Node("<repeat-statement>");

    node->addChild(new Node(ts.expect("repeatsy")));

    node->addChild(parseStatementList(ts));

    node->addChild(new Node(ts.expect("untilsy")));
    node->addChild(parseExpression(ts));

    return node;
}

Node* parseForStatement(TokenStream& ts){
    Node* node = new Node("<for-statement>");

    node->addChild(new Node(ts.expect("forsy")));
    node->addChild(new Node(ts.expect("ident")));
    node->addChild(new Node(ts.expect("becomes")));

    node->addChild(parseExpression(ts));

    if (ts.check("tosy") || ts.check("downtosy")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    } else {
        throwSyntaxError(ts.current(), "tosy/downtosy", ts.getIndex());
    }

    node->addChild(parseExpression(ts));

    node->addChild(new Node(ts.expect("dosy")));
    // node->addChild(parseStatement(ts));
    std::cout << "Sebelum compoundstatement dari for" << std::endl;
    node->addChild(parseCompoundStatement(ts));
    std::cout << "setelah compoundstatement dari for" << std::endl;
    // node->addChild(new Node(ts.expect("semicolon")));
    std::cout << "setelah semicolon dari for" << std::endl;
    std::cout << "token sekarang : " << ts.current() << std::endl;

    return node;
}

Node* parseCaseStatement(TokenStream& ts){
    Node* node = new Node("<case-statement>");

    node->addChild(new Node(ts.expect("casesy")));
    node->addChild(parseExpression(ts));
    node->addChild(new Node(ts.expect("ofsy")));

    node->addChild(parseCaseList(ts));

    node->addChild(new Node(ts.expect("endsy")));

    return node;
}

Node* parseCaseList(TokenStream& ts){
    Node* node = new Node("<case-list>");

    node->addChild(parseCaseBranch(ts));

    while (ts.check("semicolon")) {
        node->addChild(new Node(ts.current()));
        ts.advance();

        if (ts.check("endsy")) break;

        node->addChild(parseCaseBranch(ts));
    }

    return node;
}

Node* parseCaseBranch(TokenStream& ts){
    Node* node = new Node("<case-branch>");

    node->addChild(parseConstantList(ts));
    node->addChild(new Node(ts.expect("colon")));

    node->addChild(parseStatement(ts));

    return node;
}

Node* parseConstantList(TokenStream& ts){
    Node* node = new Node("<constant-list>");

    node->addChild(parseExpression(ts)); 

    while (ts.check("comma")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseExpression(ts));
    }

    return node;
}

// Node* parseExpressionList(TokenStream& ts){
//     Node* node = new Node("<expression-list>");

//     node->addChild(parseExpression(ts));

//     while (ts.check("comma")) {
//         node->addChild(new Node(ts.current()));
//         ts.advance();
//         node->addChild(parseExpression(ts));
//     }

//     return node;
// }