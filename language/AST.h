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
    BINARY_OP
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

void printAst(AstNode* node, int depth = 0);
void deleteAst(AstNode* node);


#endif //AUTOMATA3_AST_H