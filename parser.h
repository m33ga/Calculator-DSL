#ifndef CALCULATOR_DSL_PARSER_H
#define CALCULATOR_DSL_PARSER_H

/*program:
 * block =
["int:" ident {"," ident} ";"]
["double:" ident {"," ident} ";"]
statement

statement =
ident "=" expression
| "print" ident
| "{" statement {";" statement } "}"
| "if" condition "then" statement
| "while" condition "do" statement

condition =
expression ("="|"#"|"<"|"<="|">"|">=") expression .

expression = ["+"|"-"] term {("+"|"-") term} .

term = factor {("*"|"/") factor} .

factor =
ident
| number
| "(" expression ")"

*/


//#include "lexer.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <exception>

using namespace std;

int position = -1;
Token tok;
vector<Token> tokens;

void nextTok() {
    position++;
    tok = tokens[position];
}

void error(const string& message){
    cerr << message << endl;
    exit(EXIT_FAILURE);
}

struct ASTNode{
    Token token;
    ASTNode *left;
    ASTNode *right;
    vector<ASTNode*> children;
};

ASTNode *expression();

bool accept(TokenType t){
    if (tok.type == t){
        return true;
    }
    return false;
}

bool expect(TokenType tokType) {
    if (accept(tokType)){
        nextTok();
        return true;
    }
    error("Expect: Unexpected symbol, line: " + to_string(tok.line) + ", expected: " + toStr(tokType) + ", found: " + toStr(tok.type));
    return false;
}

ASTNode* factor() {
    ASTNode* node = nullptr;
    if (accept(IDENTIFIER)){
        node = new ASTNode{tok, nullptr, nullptr, {}};
        nextTok();
    } else if (accept(INT_NUMBER) || accept(DOUBLE_NUMBER)){
        node = new ASTNode{tok, nullptr, nullptr, {}};
        nextTok();
    } else if (accept(LPar)) {
        nextTok();
        node = expression();
        expect(RPar);
    } else {
        error("Factor: Syntax error, line: " + to_string(tok.line));
        nextTok();
    }
    return node;
}

ASTNode* term() {
    ASTNode* node = factor();
    while (tok.type == MULTIPLY || tok.type == DIVIDE){
        nextTok();
        node = new ASTNode{tokens[position-1], node, factor(), {}};
    }
    return node;
}


ASTNode* expression() {

    ASTNode* node = nullptr;
    if (tok.type == PLUS || tok.type == MINUS){
        node = new ASTNode{tok, nullptr, nullptr, {}};
        nextTok();
    }
    if (node == nullptr){
        node = term();
    } else {
        node->left = term();
    }
    while (tok.type == PLUS || tok.type == MINUS) {
        nextTok();
        node = new ASTNode{tokens[position-1], node, term(),{}};
    }
    return node;
}

ASTNode* condition(){
    ASTNode* left = expression();
    ASTNode* node = nullptr;
    if (tok.type == EQUAL || tok.type == DIFFERENT || tok.type == SMALLER || tok.type == SEqual || tok.type == GREATER || tok.type == GEqual) {
        nextTok();
        node = new ASTNode{tokens[position-1], left, expression(), {}};
    } else {
        error("Condition: Invalid operator, line: " + to_string(tok.line));
        nextTok();
    }
    return node;
}

ASTNode* statement() {
    if (accept(IDENTIFIER)) {
        ASTNode* assignmentNode;
        auto left = new ASTNode{tok, nullptr, nullptr, {}};
        nextTok();
        expect(ASSIGN);
        assignmentNode = new ASTNode{tokens[position-1], left, expression(), {}};
        return assignmentNode;
    } else if (accept(PRINTst)) {
        ASTNode* printNode;
        ASTNode* child;
        nextTok();
        expect(IDENTIFIER);
        printNode = new ASTNode{tokens[position-2], nullptr, nullptr, {}};
        child = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
        printNode->children.push_back(child);
        return printNode;
    } else if (accept(LBrackets)) {
        ASTNode* blockNode;
        blockNode = new ASTNode{tok, nullptr, nullptr, {}};
        do {
            nextTok();
            blockNode->children.push_back(statement());
        } while (accept(SEMICOLON));
        expect(RBrackets);
        blockNode->children.push_back(nullptr);
        return blockNode;
    } else if (accept(IFst)) {
        auto ifStatement = new ASTNode{tok, nullptr, nullptr, {}};
        ASTNode* conditionSt;
        ASTNode* thenSt;
        ASTNode* elseSt = nullptr;
        nextTok();
        conditionSt = condition();
        expect(THENst);
        thenSt = statement();
        if (accept(ELSEst)){
            nextTok();
            elseSt = statement();
        }
        ifStatement->children.insert(ifStatement->children.end(), {conditionSt, thenSt, elseSt, nullptr});
        return ifStatement;
    } else if (accept(WHILEst)) {
        auto whileSt = new ASTNode{tok, nullptr, nullptr, {}};
        nextTok();
        ASTNode* conditionSt = condition();
        expect(DOst);
        ASTNode* doSt = statement();
        whileSt->left = conditionSt;
        whileSt->right = doSt;
        return whileSt;
    } else {
        error("Statement: Syntax error, line: "+ to_string(tok.line));
        nextTok();
    }
    return nullptr;
}

ASTNode* program(){
    expect(PROGRAM);
    auto node = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
    ASTNode* intVars = nullptr;
    ASTNode* doubleVars = nullptr;
    ASTNode* programSt = nullptr;
    if (accept(INTvar)) {
        intVars = new ASTNode{tok, nullptr, nullptr, {}};
        do {
            nextTok();
            expect(IDENTIFIER);
            auto id = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
            intVars->children.push_back(id);
        } while (accept(COMMA));
        expect(SEMICOLON);
    }
    if (accept(DOUBLEvar)) {
        doubleVars = new ASTNode{tok, nullptr, nullptr, {}};
        do {

            nextTok();
            expect(IDENTIFIER);
            auto iden = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
            doubleVars->children.push_back(iden);
        } while (accept(COMMA));
        expect(SEMICOLON);
    }
    programSt = statement();
    node->left = intVars;
    node->right = doubleVars;
    node->children.push_back(programSt);
    if (position < tokens.size()){
        error("Syntax error: Unexpected token, line: " + to_string(tok.line));
    }

    return node;
}

ASTNode* parseTokens(const string& input){
    tokens = lex(input, 1);
    nextTok();
    return program();
}
#endif //CALCULATOR_DSL_PARSER_H
