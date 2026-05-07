
#ifndef AUTOMATA3_INTERPRETER_H
#define AUTOMATA3_INTERPRETER_H
#include "AST.h"
#include "VarTable.h"


class Interpreter {
private:
    VarTable varTable;
    void execute(AstNode* node);
    Value evaluate(AstNode* node);
    void executeDeclaration(AstNode* node);
    void executeAssign(AstNode* node);
    void executeIncDec(AstNode* node);
    void executeIf(AstNode* node);
    void executeWhile(AstNode* node);

    Value evaluateUnaryOp(AstNode* node);
    Value evaluateBinaryOp(AstNode* node);
    void checkBool(const Value& value);
    void checkUint(const Value& value);
public:
    void run(AstNode* root);
};


#endif //AUTOMATA3_INTERPRETER_H