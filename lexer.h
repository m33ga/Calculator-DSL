#ifndef CALCULATOR_DSL_LEXER_H
#define CALCULATOR_DSL_LEXER_H

#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;

enum TokenType {
    INTvar,
    DOUBLEvar,
    IFst,
    THENst,
    ELSEst,
    WHILEst,
    DOst,
    IDENTIFIER,
    INT_NUMBER,
    DOUBLE_NUMBER,
    ASSIGN,
    LPar,
    RPar,
    LBrackets,
    RBrackets,
    SEMICOLON,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    GREATER,
    GEqual,
    EQUAL,
    SMALLER,
    SEqual,
    DIFFERENT,
    UNKNOWN,
    PRINTst,
    COMMA,
    PROGRAM
};

struct Token {
    string value;
    TokenType type;
    int line;
};

string toStr(enum TokenType type){
    switch(type){
        case INTvar: return "INTvar";
        case DOUBLEvar: return "DOUBLEvar";
        case IFst: return "IFst";
        case ELSEst: return "ELSEst";
        case THENst: return "THENst";
        case WHILEst: return "WHILEst";
        case DOst: return "DOst";
        case IDENTIFIER: return "IDENTIFIER";
        case INT_NUMBER: return "INT_NUMBER";
        case DOUBLE_NUMBER: return "DOUBLE_NUMBER";
        case ASSIGN: return "ASSIGN";
        case LPar: return "LPar";
        case RPar: return "RPar";
        case LBrackets: return "LBrackets";
        case RBrackets: return "RBrackets";
        case SEMICOLON: return "SEMICOLON";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULTIPLY: return "MULTIPLY";
        case DIVIDE: return "DIVIDE";
        case GREATER: return "GREATER";
        case GEqual: return "GEqual";
        case EQUAL: return "EQUAL";
        case SMALLER: return "SMALLER";
        case SEqual: return "SEqual";
        case DIFFERENT: return "DIFFERENT";
        case PRINTst: return "PRINTst";
        case COMMA: return "COMMA";
        case PROGRAM: return "PROGRAM";
        default: return "UNKNOWN";
    }
}

vector<Token> lex(const string& input, int line) {
    vector<Token> tokens;
    auto doubleRegex = regex("^[0-9]+\\.[0-9]+");
    auto intRegex = regex("^[0-9]+");
    auto intvarRegex = regex("^int:");
    auto doublevarRegex = regex("^double:");
    auto whileRegex = regex("^while:");
    auto ifRegex = regex("^if:");
    auto elseRegex = regex("^else:");
    auto thenRegex = regex("^then:");
    auto doRegex = regex("^do:");
    auto programRegex = regex("^program:");
    auto printRegex = regex("^print:");
    auto identifierRegex = regex("^[a-zA-Z][a-zA-Z0-9]*");
    auto operatorsMap = map<string, TokenType>{
            {"(", LPar},
            {")", RPar},
            {"=", ASSIGN},
            {";", SEMICOLON},
            {"{", LBrackets},
            {"}", RBrackets},
            {"+", PLUS},
            {"-", MINUS},
            {"*", MULTIPLY},
            {"/", DIVIDE},
            {"<", SMALLER},
            {"<=", SEqual},
            {"==", EQUAL},
            {">=", GEqual},
            {">", GREATER},
            {"!=", DIFFERENT},
            {",", COMMA}
    };

    smatch match;
    int position = 0;
    string currentStr = input.substr(position);
    while (position < input.length()){
        if (input[position] == '\n'){
            ++line;
            ++position;
            currentStr = input.substr(position);
            continue;
        }
        if (isspace(input[position])){
            ++position;
            currentStr = input.substr(position);
            continue;
        }

        string symbol = input.substr(position, 1);
        string symbols2;
        if (position < input.length()-2) {
            symbols2 = input.substr(position, 2);

            if (operatorsMap.find(symbols2) != operatorsMap.end()) {
                tokens.push_back({symbols2, operatorsMap[symbols2], line});
                position += 2;
                currentStr = input.substr(position);
                continue;
            }
        }
        if (operatorsMap.find(symbol) != operatorsMap.end())
        {
            tokens.push_back({symbol, operatorsMap[symbol], line});
            position += 1;
            currentStr = input.substr(position);
            continue;
        }
        if (regex_search(currentStr, match, intvarRegex)){
            tokens.push_back({match.str(), INTvar, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, programRegex)){
            tokens.push_back({match.str(), PROGRAM, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, printRegex)){
            tokens.push_back({match.str(), PRINTst, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, doublevarRegex)){
            tokens.push_back({match.str(), DOUBLEvar, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, ifRegex)){
            tokens.push_back({match.str(), IFst, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, thenRegex)){
            tokens.push_back({match.str(), THENst, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, elseRegex)){
            tokens.push_back({match.str(), ELSEst, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, whileRegex)){
            tokens.push_back({match.str(), WHILEst, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, doRegex)){
            tokens.push_back({match.str(), DOst, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, identifierRegex)){
            tokens.push_back({match.str(), IDENTIFIER, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, doubleRegex)) {
            tokens.push_back({match.str(), DOUBLE_NUMBER, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        }
        if (regex_search(currentStr, match, intRegex)){
            tokens.push_back({match.str(), INT_NUMBER, line});
            position += match.length();
            currentStr = input.substr(position);

            continue;
        } else {
            tokens.push_back({"", UNKNOWN, line});
            position++;
            currentStr = input.substr(position);
            continue;
        }
    }
    return tokens;
}

#endif //CALCULATOR_DSL_LEXER_H
