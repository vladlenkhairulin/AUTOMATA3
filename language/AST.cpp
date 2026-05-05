#include "AST.h"
#include <iostream>

AstNode* createNode(NodeType type) {
    return new AstNode(type);
}

AstNode* createProgram(AstNode* statements) {
    AstNode* node = new AstNode(NodeType::PROGRAM);
    if (statements != nullptr) {
        node->children.push_back(statements);
    }
    return node;
}

AstNode* createBlock() {
    return new AstNode(NodeType::BLOCK);
}

AstNode* appendNode(AstNode* block, AstNode* node) {
    if (block == nullptr) block = createBlock();
    if (node != nullptr) block->children.push_back(node);
    return block;
}

AstNode* makeDeclaration(const std::string& typeName, const std::string& name, AstNode* value, bool isConst) {
    AstNode* node = new AstNode(NodeType::DECLARATION);
    node->dataType = typeName;
    node->name = name;
    node->isConst = isConst;
    if (value != nullptr) node->children.push_back(value);
    return node;
}

AstNode* makeAssign(const std::string& name, AstNode* value) {
    AstNode* node = new AstNode(NodeType::ASSIGN);
    node->name = name;
    if (value != nullptr) {
        node->children.push_back(value);
    }
    return node;
}

AstNode* makeIncDec(const std::string& operation, const std::string& name) {
    AstNode* node = new AstNode(NodeType::INC_DEC);
    node->op = operation;
    node->name = name;
    return node;
}

AstNode* makeNumber(unsigned value) {
    AstNode* node = new AstNode(NodeType::NUMBER);
    node->intValue = value;
    return node;
}

AstNode* makeBool(bool value) {
    AstNode* node = new AstNode(NodeType::BOOL_VALUE);
    node->boolValue = value;
    return node;
}

AstNode* makeVar(const std::string& name) {
    AstNode* node = new AstNode(NodeType::VAR);
    node->name = name;
    return node;
}

AstNode* makeRobotCmd(const std::string& cmd) {
    AstNode* node = new AstNode(NodeType::ROBOT_CMD);
    node->op = cmd;
    return node;
}

AstNode* makeLocateCmd(const std::string& cmd) {
    AstNode* node = new AstNode(NodeType::LOCATE_CMD);
    node->op = cmd;
    return node;
}

AstNode* makeUnaryOperation(const std::string& operation, AstNode* operand) {
    AstNode* node = new AstNode(NodeType::UNARY_OP);
    node->op = operation;
    if (operand != nullptr) node->children.push_back(operand);
    return node;
}

AstNode* makeBinaryOperation(const std::string& operation, AstNode* left, AstNode* right) {
    AstNode* node = new AstNode(NodeType::BINARY_OP);
    node->op = operation;
    if (left != nullptr) {
        node->children.push_back(left);
    }
    if (right != nullptr) {
        node->children.push_back(right);
    }
    return node;
}

void printIndent(int depth) {
    for (int i = 0; i < depth; i++) std::cout << "    ";
}

void printAst(AstNode* node, int depth) {
    if (node == nullptr) return;
    printIndent(depth);
    switch (node->nodeType) {
        case NodeType::PROGRAM:
            std::cout << "PROGRAM";
            break;
        case NodeType::BLOCK:
            std::cout << "BLOCK";
            break;
        case NodeType::DECLARATION:
            std::cout << (node->isConst ? "const" : "") << node->dataType << node->name;
            break;
        case NodeType::ASSIGN:
            std::cout << "ASSIGN " << node->name;
            break;
        case NodeType::INC_DEC:
            std::cout << "INC_DEC op = " << node->op << " var = " << node->name;
            break;
        case NodeType::NUMBER:
            std::cout << "NUMBER " << node->intValue;
            break;
        case NodeType::BOOL_VALUE:
            std::cout << "BOOL " << (node->boolValue ? "true" : "false");
            break;
        case NodeType::VAR:
            std::cout << "VAR " << node->name;
            break;
        case NodeType::ROBOT_CMD:
            std::cout << "ROBOT_CMD " << node->op;
            break;
        case NodeType::LOCATE_CMD:
            std::cout << "LOCATE_CMD " << node->op;
            break;
        case NodeType::UNARY_OP:
            std::cout << "UNARY_OP " << node->op;
            break;
        case NodeType::BINARY_OP:
            std::cout << "BINARY_OP " << node->op;
            break;
    }

    std::cout << std::endl;
    for (AstNode* child : node->children) printAst(child, depth + 1);
}

void deleteAst(AstNode* node) {
    if (node == nullptr) return;
    for (AstNode* child : node->children) {
        deleteAst(child);
    }
    delete node;
}