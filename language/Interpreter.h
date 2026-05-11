
#ifndef AUTOMATA3_INTERPRETER_H
#define AUTOMATA3_INTERPRETER_H
#include "AST.h"
#include "VarTable.h"
#include <unordered_map>
#include <vector>
#include "../game/Robot.h"


class Interpreter {
private:
    VarTable varTable;
    std::unordered_map<std::string, AstNode*> functions;
    Robot* game = nullptr;
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
    void executeArrDeclaration(AstNode* node);
    void executeArrSet(AstNode* node);
    void executeArrExtend(AstNode* node);

    Value evaluateFuncCall(AstNode* node);
    std::vector<Value> evaluateArgs(AstNode *argsBlock);
    Value evaluateIncDec(AstNode* node);
    Value evaluateUnaryOp(AstNode* node);
    Value evaluateBinaryOp(AstNode* node);
    Value evaluateArrSize(AstNode* node);
    Value evaluateArrGet(AstNode* node);

    std::vector<Value> callFunction(const std::string &name, AstNode *args);
    void checkBool(const Value& value);
    void checkUint(const Value& value);
    unsigned int getIndex(AstNode* node);
    Value getDefaultType(ValueType type);
public:
    void run(AstNode* root);
    explicit Interpreter(Robot* game) {
        this->game = game;
    };
};


#endif //AUTOMATA3_INTERPRETER_H