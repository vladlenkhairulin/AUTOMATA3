
#ifndef AUTOMATA3_INTERPRETER_H
#define AUTOMATA3_INTERPRETER_H
#include "AST.h"
#include "VarTable.h"
#include <unordered_map>
#include <vector>


class Interpreter {
private:
    VarTable varTable;
    std::unordered_map<std::string, AstNode*> functions;
    void execute(AstNode* node);
    Value evaluate(AstNode* node);
    void executeDeclaration(AstNode* node);
    void executeAssign(AstNode* node);
    void executeIncDec(AstNode* node);
    void executeIf(AstNode* node);
    void executeWhile(AstNode* node);
    void executeFuncDeclaration(AstNode* node);
    void executeFuncCallAssign(AstNode* node);
    void executeFuncCall(AstNode* node);
    Value evaluateFuncCall(AstNode* node);

    std::vector<Value> evaluateArgs(AstNode *argsBlock);

    Value evaluateIncDec(AstNode* node);

    std::vector<Value> callFunction(const std::string &name, AstNode *args);

    Value evaluateUnaryOp(AstNode* node);
    Value evaluateBinaryOp(AstNode* node);
    void checkBool(const Value& value);
    void checkUint(const Value& value);
public:
    void run(AstNode* root);
};


#endif //AUTOMATA3_INTERPRETER_H