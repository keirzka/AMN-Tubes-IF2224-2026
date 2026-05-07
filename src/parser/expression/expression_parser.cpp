#include "expression_parser.hpp"
#include "parser/utils/error.hpp"


Node* parseExpression(TokenStream& ts){
    Node* node = new Node("<expression>");

    node->addChild(parseSimpleExpression(ts));

    while (ts.check("eql") || ts.check("neq") || ts.check("gtr") || ts.check("geq") || ts.check("lss") || ts.check("leq")) {
        node->addChild(new Node (ts.current()));
        ts.advance();

        node->addChild(parseSimpleExpression(ts));
    }

    return node;
}


Node* parseSimpleExpression(TokenStream& ts){
    Node* node = new Node("<simple-expression>");

    while (ts.check("plus") || ts.check("minus") || ts.check("orsy")) {
        node->addChild(new Node (ts.current()));
        ts.advance();
    }

    node->addChild(parseTerm(ts));

    while (ts.check("plus") || ts.check("minus") || ts.check("orsy")) {
        // node->addChild(new Node(ts.current())); // operator
        node->addChild(parseAdditiveOperator(ts));
        // ts.advance();

        node->addChild(parseTerm(ts));
    }

    return node;
}

Node* parseTerm(TokenStream& ts){
    Node* node = new Node("<term>");

    node->addChild(parseFactor(ts));

    while (ts.check("times") || ts.check("rdiv") || ts.check("idiv") || ts.check("imod") || ts.check("andsy")) {
        node->addChild(new Node(ts.current())); // operator
        ts.advance();

        node->addChild(parseFactor(ts));
    }

    return node;
}

Node* parseVariableTanpaIdent (TokenStream& ts) {
    Node* node = new Node ("<variable>");

    while (ts.check("lbrack") || ts.check("period")) {
        // std::cout << "masuk loop" << std::endl;
        node->addChild(parseComponentVariable(ts));
    }

    // node->addChild(parseComponentVariable(ts));

    return node;
}

Node* parseProcedureCallTanpaIdent (TokenStream& ts) {
    Node* node = new Node("<procedure-call>");

    if (ts.check("lparent")) {
        node->addChild(new Node(ts.current()));
        ts.advance();

        if (!ts.check("rparent")) {
            node->addChild(parseParameterList(ts));
        }

        node->addChild(new Node(ts.expect("rparent", "procedure call")));
    }

    while (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    }

    return node;
}

Node* parseFactor(TokenStream& ts){
    Node* node = new Node("<factor>");

    if (ts.check("intcon") || ts.check("realcon") || ts.check("charcon") || ts.check("string")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    else if (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();

        if (ts.check("lbrack") || ts.check("period")) { // variabel
            node->addChild(parseVariableTanpaIdent(ts));
        }
        else if (ts.check("lparent")) { // procedure call
            node->addChild(parseProcedureCallTanpaIdent(ts));
        }
    }
    else if (ts.check("lparent")) {
        node->addChild(new Node(ts.current())); // (
        ts.advance();

        node->addChild(parseExpression(ts));

        node->addChild(new Node(ts.expect("rparent", "factor")));
    }
    else if (ts.check("notsy")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
        node->addChild(parseFactor(ts));
    }
    else {
        throwSyntaxError(ts.current(), "factor", ts.getIndex());
    }

    return node;
}

Node* parseVariable (TokenStream& ts) {
    if (ts.getIndex() > 90) {
        std::cout << "masuk vairabel\n";
        std::cout << ts.current() << std::endl;
    }
    Node* node = new Node ("<variable>");

    if (ts.check("ident")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    // else {
    std::cout << "masuk sini" << std::endl;
    while (ts.check("lbrack") || ts.check("period")) {
        std::cout << "masuk loop" << std::endl;
        node->addChild(parseComponentVariable(ts));
    }
    // }

    return node;
}

Node* parseComponentVariable (TokenStream& ts) {
    Node* node = new Node ("<component-variable>");

    // node->addChild(parseVariable(ts));

    if (ts.check("lbrack")) {
        node->addChild (new Node (ts.current()));
        ts.advance();

        node->addChild (parseIndexList(ts));

        if (ts.check("rbrack")) {
            node->addChild (new Node (ts.current()));
            ts.advance();
        }
        else {
            throwSyntaxError(ts.current(), "rbrack", ts.getIndex());
        }
    }
    else if (ts.check("period")) {
        node->addChild (new Node (ts.current()));
        ts.advance();

        if (ts.check("ident")) {
            node->addChild (new Node (ts.current()));
            ts.advance();
        }
        else {
            throwSyntaxError(ts.current(), "ident", ts.getIndex());
        }
    }
    else {
        throwSyntaxError(ts.current(), "[ or .", ts.getIndex());
    }

    return node;
}

Node* parseIndexList (TokenStream& ts) {
    Node* node = new Node ("<index-list>");

    if (ts.check("intcon") || ts.check("charcon") || ts.check("ident")) {
        node->addChild (new Node(ts.current()));
        ts.advance();

        while (ts.check("comma")) {
            node->addChild (new Node(ts.current()));
            ts.advance();

            node->addChild (parseIndexList(ts));
        }
    }
    else {
        throwSyntaxError(ts.current(), "intcon, charcon, ident", ts.getIndex());
    }

    return node;
}

// Node* parseAssignmentStatement (TokenStream& ts) {
//     Node* node = new Node ("<assignment-statement>");

//     node->addChild(parseVariable(ts));

//     if (ts.check("becomes")) {
//         node->addChild(new Node (ts.current()));
//         ts.advance();

//         node->addChild(parseExpression(ts));
//     }
//     else {
//         throwSyntaxError (ts.current(), "becomes", ts.getIndex());
//     }

//     return node;
// }

Node* parseParameterList (TokenStream& ts) {
    Node* node = new Node ("<parameter-list>");

    node->addChild(parseExpression(ts));

    while (ts.check("comma")) {
        node->addChild(new Node (ts.current()));
        ts.advance();

        node->addChild(parseExpression(ts));
    }

    return node;
}

Node* parseRelationalOperator (TokenStream& ts) {
    Node* node = new Node ("<relational-operator>");
    // eql | neq | gtr | geq | lss | leq
    if (ts.check("eql") || ts.check("neq") || ts.check("gtr") || ts.check("geq") || ts.check("lss") || ts.check("leq")) {
        node->addChild (new Node(ts.current()));
        ts.advance();
    }
    else {
        throwSyntaxError(ts.current(), "relational operator", ts.getIndex());
    }

    return node;
}

Node* parseAdditiveOperator (TokenStream& ts) {
    Node* node = new Node ("<additive-operator>");
    // eql | neq | gtr | geq | lss | leq
    if (ts.check("plus") || ts.check("minus") || ts.check("orsy")) {
        node->addChild (new Node(ts.current()));
        ts.advance();
    }
    else {
        throwSyntaxError(ts.current(), "additive operator", ts.getIndex());
    }

    return node;
}

Node* parseMultiplicativeOperator (TokenStream& ts) {
    Node* node = new Node ("<multiplicative-operator>");
    // eql | neq | gtr | geq | lss | leq
    if (ts.check("times") || ts.check("rdiv") || ts.check("idiv") || ts.check("imod") || ts.check("andsy")) {
        node->addChild (new Node(ts.current()));
        ts.advance();
    }
    else {
        throwSyntaxError(ts.current(), "multiplicative operator", ts.getIndex());
    }

    return node;
}
