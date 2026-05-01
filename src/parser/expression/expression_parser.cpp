#include "expression_parser.hpp"
#include "parser/utils/error.hpp"

/**
 * <expression> → <simple-expression>
 */
Node* parseExpression(TokenStream& ts){
    Node* node = new Node("<expression>");

    node->addChild(parseSimpleExpression(ts));

    // TODO (Fakhry):
    // Tambahkan relop handling:
    // if (ts.check("eql") || ts.check("neq") ...)
    //     parseSimpleExpression lagi

    return node;
}

/**
 * <simple-expression> → <term> (addop <term>)*
 */
Node* parseSimpleExpression(TokenStream& ts){
    Node* node = new Node("<simple-expression>");

    node->addChild(parseTerm(ts));

    while (ts.check("plus") || ts.check("minus") || ts.check("orsy")) {
        node->addChild(new Node(ts.current())); // operator
        ts.advance();

        node->addChild(parseTerm(ts));
    }

    return node;
}

/**
 * <term> → <factor> (mulop <factor>)*
 */
Node* parseTerm(TokenStream& ts){
    Node* node = new Node("<term>");

    node->addChild(parseFactor(ts));

    while (ts.check("times") || ts.check("rdiv") || ts.check("idiv") || ts.check("andsy")) {
        node->addChild(new Node(ts.current())); // operator
        ts.advance();

        node->addChild(parseFactor(ts));
    }

    return node;
}

/**
 * <factor> →
 *      ident
 *    | intcon
 *    | realcon
 *    | (expression)
 */
Node* parseFactor(TokenStream& ts){
    Node* node = new Node("<factor>");

    if (ts.check("ident") || ts.check("intcon") || ts.check("realcon")) {
        node->addChild(new Node(ts.current()));
        ts.advance();
    }
    else if (ts.check("lparent")) {
        node->addChild(new Node(ts.current())); // (
        ts.advance();

        node->addChild(parseExpression(ts));

        node->addChild(new Node(ts.expect("rparent", "factor")));
    }
    else {
        throwSyntaxError(ts.current(), "factor", ts.getIndex());
    }

    return node;
}