#include <iostream>
#include <cstdio>
#include "language/AST.h"
#include "language/Interpreter.h"
extern int yyparse();
extern FILE* yyin;
AstNode* getRoot();

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "start the program correctly: must be 2 args" << std::endl;
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cout << "fopen error: " << argv[1] << std::endl;
        return 1;
    }
    int res = yyparse();
    fclose(yyin);
    if (res == 0) std::cout << "Parsed successfully" << std::endl;
    else {
        std::cout << "Parsing error" << std::endl;
        return 1;
    }
    AstNode* root = getRoot();
    std::cout << "AST:" << std::endl;

    printAst(root);
    std::cout << std::endl;
    std::cout << "Interpreter:\n";
    try {
        Interpreter interpreter;
        interpreter.run(root);
    } catch (const std::exception& e) {
        std::cout << "Interpreting error!" << e.what() << std::endl;
    }
    deleteAst(root);
    return res;
}