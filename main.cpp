#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <fstream>

int main(){
    string programLine;
    string program = "";
    ifstream sourceFile("source.txt");
    while (getline(sourceFile, programLine)){
        program += programLine + '\n';
    }
    interpretProgram(parseTokens(program));
    return 0;
}