#include "AST.h"
#include <iostream>
#include <bits/locale_facets_nonio.h>

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

AstNode* makeIf(AstNode* cond, AstNode* thenNode, AstNode* elseNode) {
    AstNode* node = new AstNode(NodeType::IF);
    if (cond != nullptr) node->children.push_back(cond);
    if (thenNode != nullptr) node->children.push_back(thenNode);
    if (elseNode != nullptr) node->children.push_back(elseNode);
    return node;
}

AstNode* makeWhile(AstNode *cond, AstNode *inner) {
    AstNode* node = new AstNode(NodeType::WHILE);
    if (cond != nullptr) node->children.push_back(cond);
    if (inner != nullptr) node->children.push_back(inner);
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
        case NodeType::IF:
            std::cout << "IF ";
            break;
        case NodeType::WHILE:
            std::cout << "WHILE ";
            break;
        case NodeType::ARR_DECL:
            std::cout << "ARR_DECL " << node->dataType << " " << node->name;
            break;
        case NodeType::ARR_SET:
            std::cout << "ARR_SET " << node->name;
            break;
        case NodeType::ARR_EXTEND:
            std::cout << "ARR " << node->op << " " << node->name;
            break;
        case NodeType::ARR_SIZE:
            std::cout << "ARR " << node->op << " " << node->name;
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

AstNode* makeFuncDecl(const std::string& name, AstNode* returns, AstNode* params, AstNode* body) {
    AstNode* node = new AstNode(NodeType::FUNC_DECL);
    node->name = name;
    if (returns != nullptr) node->children.push_back(returns);
    else node->children.push_back(createBlock());
    if (params != nullptr) node->children.push_back(params);
    else node->children.push_back(createBlock());
    if (body != nullptr) node->children.push_back(body);
    else node->children.push_back(createBlock());
    return node;
}

AstNode* makeFuncCall(const std::string& name, AstNode* args) {
    AstNode* node = new AstNode(NodeType::FUNC_CALL);
    node->name = name;
    if (args != nullptr) node->children.push_back(args);
    else node->children.push_back(createBlock());
    return node;
}

AstNode* makeFuncCallAssign(const std::string& name, AstNode* targets, AstNode* args) {
    AstNode* node = new AstNode(NodeType::FUNC_CALL_ASSIGN);
    node->name = name;
    if (targets != nullptr) node->children.push_back(targets);
    else node->children.push_back(createBlock());
    if (args != nullptr) node->children.push_back(args);
    else node->children.push_back(createBlock());
    return node;
}

AstNode* makeFuncParam(const std::string& name, AstNode* defaultValue) {
    AstNode* node = new AstNode(NodeType::FUNC_PARAM);
    node->name = name;
    if (defaultValue != nullptr) node->children.push_back(defaultValue);
    return node;
}

AstNode* makeFuncReturn(const std::string& name, AstNode* defaultValue) {
    AstNode* node = new AstNode(NodeType::FUNC_RETURN);
    node->name = name;
    if (defaultValue != nullptr) node->children.push_back(defaultValue);
    return node;
}

AstNode* makeFuncEmpty() {
    return new AstNode(NodeType::FUNC_EMPTY);
}

AstNode* makeArrayDeclaration(const std::string& typeName, const std::string& name, AstNode* values) {
    AstNode* node = new AstNode(NodeType::ARR_DECL);
    node->dataType = typeName;
    node->name = name;
    if (values != nullptr) node->children.push_back(values);
    return node;
}

AstNode *makeArraySet(const std::string &name, AstNode *indexes, AstNode *value) {
    AstNode* node = new AstNode(NodeType::ARR_SET);
    node->name = name;
    if (indexes != nullptr) node->children.push_back(indexes);
    if (value != nullptr) node->children.push_back(value);
    return node;
}

AstNode* makeArrayExtend(const std::string& op, const std::string& name, AstNode* first, AstNode* second) {
    AstNode* node = new AstNode(NodeType::ARR_EXTEND);
    node->op = op;
    node->name = name;
    if (first != nullptr) node->children.push_back(first);
    if (second != nullptr) node->children.push_back(second);
    return node;
}

AstNode* makeArraySize(const std::string& op, const std::string& name, AstNode* index) {
    AstNode* node = new AstNode(NodeType::ARR_SIZE);
    node->op = op;
    node->name = name;
    if (index != nullptr) node->children.push_back(index);
    return node;
}



