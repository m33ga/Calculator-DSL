/*
#include "lexer.h"
#include "parser.h"

map<string, int> intVars;
map<string, double> doubleVars;

void interpretIntDeclaration(ASTNode* node){
    for (const auto& child:node->children){
        if (intVars.find(child->token.value) != intVars.end()){
            error("Variable already exists: " + child->token.value);
        }
        intVars.insert({child->token.value, 0});
    }
}

void interpretDoubleDeclaration(ASTNode* node){
    for (const auto& child:node->children){
        if (intVars.find(child->token.value) != intVars.end()){
            error("Variable already exists: " + child->token.value);
        }
        if (doubleVars.find(child->token.value) != doubleVars.end()){
            error("Variable already exists: " + child->token.value);
        }
        doubleVars.insert({child->token.value, 0});
    }
}

int interpretIntIdentifier(ASTNode* node){
    return intVars[node->token.value];
}

int interpretIntNumber(ASTNode* node){
    return stoi(node->token.value);
}

double interpretDoubleIdentifier(ASTNode* node){
    return doubleVars[node->token.value];
}

double interpretDoubleNumber(ASTNode* node){
    return stod(node->token.value);
}

double interpretDoubleExpression(ASTNode* node){
    if (node->token.type == IDENTIFIER){
        if (intVars.find(node->token.value) != intVars.end()){
            return interpretIntIdentifier(node);
        }
        if (doubleVars.find(node->token.value) != doubleVars.end()){
            return interpretDoubleIdentifier(node);
        }
    } else if (node->token.type == INT_NUMBER){
        return interpretIntNumber(node);
    } else if (node->token.type == DOUBLE_NUMBER){
        return interpretDoubleNumber(node);
    }
    if (node->token.type == PLUS && node->right == nullptr){
        return interpretDoubleExpression(node->left);
    }
    if (node->token.type == MINUS && node->right == nullptr){
        return 0-interpretDoubleExpression(node->left);
    }
    if (node->token.type == PLUS){
        return interpretDoubleExpression(node->left) + interpretDoubleExpression(node->right);
    }
    if (node->token.type == MINUS){
        return interpretDoubleExpression(node->left) - interpretDoubleExpression(node->right);
    }
    if (node->token.type == MULTIPLY){
        return interpretDoubleExpression(node->left) * interpretDoubleExpression(node->right);
    }
    if (node->token.type == DIVIDE){
        if (interpretDoubleExpression(node->right) != 0){
            return interpretDoubleExpression(node->left) / interpretDoubleExpression(node->right);
        } else {
            error("Division by 0, line: " + to_string(node->token.line));
        }
    }
    return 0;
}

int interpretIntExpression(ASTNode* node){
    if (node->token.type == IDENTIFIER){
        if (intVars.find(node->token.value) != intVars.end()){
            return interpretIntIdentifier(node);
        }
        if (doubleVars.find(node->token.value) != doubleVars.end()){
            error("Can't assign double to int, line: " + to_string(node->token.line));
        }
    } else if (node->token.type == INT_NUMBER){
        return interpretIntNumber(node);
    } else if (node->token.type == DOUBLE_NUMBER){
        error("Can't assign double to int, line: " + to_string(node->token.line));
    }
    if (node->token.type == PLUS && node->right == nullptr){
        return interpretIntExpression(node->left);
    }
    if (node->token.type == MINUS && node->right == nullptr){
        return 0-interpretIntExpression(node->left);
    }
    if (node->token.type == PLUS){
        return interpretIntExpression(node->left) + interpretIntExpression(node->right);
    }
    if (node->token.type == MINUS){
        return interpretIntExpression(node->left) - interpretIntExpression(node->right);
    }
    if (node->token.type == MULTIPLY){
        return interpretIntExpression(node->left) * interpretIntExpression(node->right);
    }
    if (node->token.type == DIVIDE){
        if (interpretIntExpression(node->right) != 0){
            return interpretIntExpression(node->left) / interpretIntExpression(node->right);
        } else {
            error("Division by 0, line: " + to_string(node->token.line));
        }
    }
    return 0;
}

bool interpretCondition(ASTNode* node){
    if (node->token.type == GREATER){
        return interpretDoubleExpression(node->left) > interpretDoubleExpression(node->right);
    }
    if (node->token.type == GEqual){
        return interpretDoubleExpression(node->left) >= interpretDoubleExpression(node->right);
    }
    if (node->token.type == EQUAL){
        return interpretDoubleExpression(node->left) == interpretDoubleExpression(node->right);
    }
    if (node->token.type == DIFFERENT){
        return interpretDoubleExpression(node->left) != interpretDoubleExpression(node->right);
    }
    if (node->token.type == SMALLER){
        return interpretDoubleExpression(node->left) < interpretDoubleExpression(node->right);
    }
    if (node->token.type == SEqual){
        return interpretDoubleExpression(node->left) <= interpretDoubleExpression(node->right);
    } else {
        error("Invalid condition, line: " + to_string(node->token.line));
    }
}

void interpretStatement(ASTNode* node){
    if (node->token.type == ASSIGN){
        if (intVars.find(node->left->token.value) != intVars.end()){
            intVars[node->left->token.value] = interpretIntExpression(node->right);
        } else if (doubleVars.find(node->left->token.value) != doubleVars.end()){
            doubleVars[node->left->token.value] = interpretDoubleExpression(node->right);
        } else {
            error("Unknown variable, line: " + to_string(node->token.line));
        }
    }
    if (node->token.type == PRINTst){
        if (intVars.find(node->children[0]->token.value) != intVars.end()){
            cout << intVars[node->children[0]->token.value] << endl;
        } else if (doubleVars.find(node->children[0]->token.value) != doubleVars.end()){
            cout << doubleVars[node->children[0]->token.value] << endl;
        }
    }
    if (node->token.type == LBrackets){
        int childNr = 0;
        while (node->children[childNr] != nullptr){
            interpretStatement(node->children[childNr]);
            ++childNr;
        }
    }
    if (node->token.type == IFst){
        bool condition = interpretCondition(node->children[0]);
        if (condition){
            interpretStatement(node->children[1]);
        } else{
            interpretStatement(node->children[2]);
        }
    }
    if (node->token.type ==WHILEst){
        bool condition = interpretCondition(node->left);
        while (condition){
            interpretStatement(node->right);
            condition = interpretCondition(node->left);
        }
    }
}

void interpretProgram(ASTNode* node){
    if (node->left != nullptr){
        interpretIntDeclaration(node->left);

    }
    if (node->right != nullptr){
        interpretDoubleDeclaration(node->right);

    }
    interpretStatement(node->children[0]);
}

int main(){
    string txt = "program: \n int: x; \n double: z; \n {x = 2+2*3; \n z = 2*x; \n while: x < z do: {x = x + 1; print: x}}";
    interpretProgram(parseTokens(txt));
    return 0;
}
 */