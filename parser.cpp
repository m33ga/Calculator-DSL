/*
 program = block

 block =
     ["var" ident {"," ident} ";"]
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

/*
#include "lexer.h"
#include <iostream>
#include <string>
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

ASTNode* createNode(Token tokenNode){
    ASTNode* newNode = new ASTNode{tokenNode, nullptr, nullptr, {}};
    return newNode;
}

void addChild(ASTNode* parent, ASTNode* child){
    parent->children.push_back(child);
}

bool accept(TokenType t){
    if (tok.type == t){
        //nextTok();
        return true;
    }
    return false;
}

bool expect(TokenType tokType) {
    if (accept(tokType)){
        nextTok();
        return true;
    }
    error("expect: unexpected symbol, line: " + to_string(tok.line) + " expected: " + toStr(tokType) + " found: " + tok.value);
    return false;
}

ASTNode* factor() {
    ASTNode* node = nullptr;
    if (accept(IDENTIFIER)) {
        node = createNode(tok);
        nextTok();
    } else if (accept(INT_NUMBER) || accept(DOUBLE_NUMBER)){
        node = createNode(tok);
        nextTok();
    } else if (accept(LPar)) {
        nextTok();
        node = expression();
        expect(RPar);
    } else {
        error("factor: syntax error, line: " + to_string(tok.line) + tok.value);
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


//when interpreting if right child is null then unary operator
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

ASTNode* condition() {
    ASTNode* left = expression();
    ASTNode* node = nullptr;
    if (tok.type == EQUAL || tok.type == DIFFERENT || tok.type == SMALLER || tok.type == SEqual || tok.type == GREATER || tok.type == GEqual) {
        nextTok();
        node = new ASTNode{tokens[position-1], left, expression(), {}};
    } else {
        error("condition: invalid operator, line: " + to_string(tok.line));
        nextTok();
    }
    return node;
}

ASTNode* statement() {
    if (accept(IDENTIFIER)) {
        ASTNode* assignmentNode;
        ASTNode* left = new ASTNode{tok, nullptr, nullptr, {}};
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
        return blockNode;
    } else if (accept(IFst)) {
        ASTNode* ifStatement = new ASTNode{tok, nullptr, nullptr, {}};
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
        ifStatement->children.insert(ifStatement->children.end(), {conditionSt, thenSt, elseSt});
        return ifStatement;
    } else if (accept(WHILEst)) {
        ASTNode* whileSt = new ASTNode{tok, nullptr, nullptr, {}};
        nextTok();
        ASTNode* conditionSt = condition();
        expect(DOst);
        ASTNode* doSt = statement();
        whileSt->left = conditionSt;
        whileSt->right = doSt;
        return whileSt;
    } else {
        error("statement: syntax error, line: "+ to_string(tok.line) + tok.value + toStr(tok.type));
        nextTok();
    }
    return nullptr;
}

ASTNode* program(){
    expect(PROGRAM);
    ASTNode* node = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
    ASTNode* intVars = nullptr;
    ASTNode* doubleVars = nullptr;
    ASTNode* programSt;
    if (accept(INTvar)) {
        do {
            intVars = new ASTNode{tok, nullptr, nullptr, {}};
            nextTok();
            expect(IDENTIFIER);
            ASTNode* id = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
            intVars->children.push_back(id);
        } while (accept(COMMA));
        expect(SEMICOLON);
    }
    if (accept(DOUBLEvar)) {
        do {
            doubleVars = new ASTNode{tok, nullptr, nullptr, {}};
            nextTok();
            expect(IDENTIFIER);
            ASTNode* iden = new ASTNode{tokens[position-1], nullptr, nullptr, {}};
            doubleVars->children.push_back(iden);
        } while (accept(COMMA));
        expect(SEMICOLON);
    }
    programSt = statement();
    node->left = intVars;
    node->right = doubleVars;
    node->children.push_back(programSt);
    return node;
}

ASTNode* parseTokens(const string& input){
    tokens = lex(input, 1);
    nextTok();
    return program();
}
*/

/*
int main(){
    string text = "program: \n int: x; double: y; \n{ x = 2*y*x*3*5/8*(9+3); if: x>y then: \n{ x=x+1; \n x = 2 * x } \n else: {y = y-1}; \n print: x}";
    parseTokens(text);
    //for (const auto& token : tokens){
      //  cout << token.value << " " << toStr(token.type) << " " << token.line << " ";
    //}
}
*/

