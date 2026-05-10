#ifndef AUTOMATA3_AST_H
#define AUTOMATA3_AST_H
#include <string>
#include <vector>

enum class NodeType {
    PROGRAM,
    BLOCK,
    DECLARATION,
    ASSIGN,
    INC_DEC,
    NUMBER,
    BOOL_VALUE,
    VAR,
    ROBOT_CMD,
    LOCATE_CMD,
    UNARY_OP,
    BINARY_OP,
    IF,
    WHILE,
    FUNC_DECL,
    FUNC_CALL,
    FUNC_CALL_ASSIGN,
    FUNC_PARAM,
    FUNC_RETURN,
    FUNC_EMPTY,
    ARR_DECL,
    ARR_SET,
    ARR_EXTEND,
    ARR_SIZE
};

struct AstNode {
    NodeType nodeType;
    std::string dataType;
    std::string op;
    std::string name;
    unsigned int intValue = 0;
    bool boolValue = false;
    bool isConst = false;
    std::vector<AstNode*> children;

    explicit AstNode(NodeType type) : nodeType(type) {};
};


AstNode* createNode(NodeType type);

AstNode* createProgram(AstNode *statements);

AstNode* createBlock();
AstNode* appendNode(AstNode* block, AstNode* node);

AstNode* makeDeclaration(const std::string& typeName, const std::string& name, AstNode* value, bool isConst);
AstNode* makeAssign(const std::string& name, AstNode* value);
AstNode* makeIncDec(const std::string& operation, const std::string& name);
AstNode* makeNumber(unsigned value);
AstNode* makeBool(bool value);
AstNode* makeVar(const std::string& name);
AstNode* makeRobotCmd(const std::string& cmd);
AstNode* makeLocateCmd(const std::string& cmd);
AstNode* makeUnaryOperation(const std::string& operation, AstNode* operand);
AstNode* makeBinaryOperation(const std::string& operation, AstNode* left, AstNode* right);
AstNode* makeIf(AstNode* cond, AstNode* ifNode, AstNode* elseNode);
AstNode* makeWhile(AstNode* cond, AstNode* inner);

AstNode* makeFuncDecl(const std::string& name, AstNode* returns, AstNode* params, AstNode* body);
AstNode* makeFuncCall(const std::string& name, AstNode* args);
AstNode* makeFuncCallAssign(const std::string& name, AstNode* targets, AstNode* args);
AstNode* makeFuncParam(const std::string& name, AstNode* defaultValue);
AstNode* makeFuncReturn(const std::string& name, AstNode* defaultValue);
AstNode* makeFuncEmpty();
void printAst(AstNode* node, int depth = 0);
void deleteAst(AstNode* node);

AstNode* makeArrayDeclaration(const std::string& typeName, const std::string& name, AstNode* values);
AstNode* makeArraySet(const std::string& name, AstNode* indexes, AstNode* value);
AstNode* makeArrayExtend(const std::string& op, const std::string& name, AstNode* first, AstNode* second);
AstNode* makeArraySize(const std::string& op, const std::string& name, AstNode* index);

#endif //AUTOMATA3_AST_H