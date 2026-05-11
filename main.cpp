#include <iostream>
#include <cstdio>
#include "language/AST.h"
#include "language/Interpreter.h"
#include "game/Robot.h"

extern int yyparse();
extern FILE* yyin;
AstNode* getRoot();

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "start the program correctly: must be 3 args" << std::endl;
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
    //std::cout << "AST:" << std::endl;

    //printAst(root);
    //std::cout << std::endl;

    Robot game;
    if (!game.loadGameInfo(argv[2])) {
        std::cout << "load game info file error" << std::endl;
        deleteAst(root);
        return 1;
    }
    game.render();
    //std::cout << "Interpreter:\n";
    try {
        Interpreter interpreter(&game);
        interpreter.run(root);
    } catch (const std::exception& e) {
        std::cout << "Interpreting error!" << e.what() << std::endl;
    }
    deleteAst(root);
    return res;
}