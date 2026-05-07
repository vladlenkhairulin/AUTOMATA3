
#include "Interpreter.h"
#include <iostream>
#include <stdexcept>

void Interpreter::checkBool(const Value &value) {
    if (value.type != ValueType::BOOLEAN) throw std::runtime_error("Runtime error: expected BOOLEAN");
}

void Interpreter::checkUint(const Value &value) {
    if (value.type != ValueType::UINT) throw std::runtime_error("Runtime error: expected UINT");
}

void Interpreter::run(AstNode* root) {
    execute(root);
    varTable.printVarTable();
}

void Interpreter::execute(AstNode* node) {
    if (node == nullptr) return;
    switch (node->nodeType) {
        case NodeType::PROGRAM:
        case NodeType::BLOCK:
            for (AstNode* child : node->children) execute(child);
            break;
        case NodeType::DECLARATION:
            executeDeclaration(node);
            break;
        case NodeType::ASSIGN:
            executeAssign(node);
            break;
        case NodeType::INC_DEC:
            executeIncDec(node);
            break;
        case NodeType::IF:
            executeIf(node);
            break;
        case NodeType::WHILE:
            executeWhile(node);
            break;
        case NodeType::ROBOT_CMD:
            evaluate(node);
            break;
        default:
            throw std::runtime_error("Unknown node type");
    }
}

Value Interpreter::evaluate(AstNode* node) {
    if (node == nullptr) return Value::makeNone();
    switch (node->nodeType) {
        case NodeType::NUMBER:
            return Value::makeUint(node->intValue);
        case NodeType::BOOL_VALUE:
            return Value::makeBool(node->boolValue);
        case NodeType::VAR:
            return varTable.getVar(node->name).value;
        case NodeType::UNARY_OP:
            return evaluateUnaryOp(node);
        case NodeType::BINARY_OP:
            return evaluateBinaryOp(node);
        case NodeType::ROBOT_CMD:
            return Value::makeBool(true);
        case NodeType::LOCATE_CMD:
            return Value::makeUint(0);
        default:
            throw std::runtime_error("Unknown node type in evaluate");
    }
}

void Interpreter::executeDeclaration(AstNode* node) {
    if (node->children.empty()) {
        throw std::runtime_error("Missing value in declaration");
    }
    Value val = evaluate(node->children[0]);
    if (node->dataType == "UINT" && val.type != ValueType::UINT) {
        throw std::runtime_error("Runtime error: UINT declaration needs UINT value: " + node->name);
    }
    if (node->dataType == "BOOLEAN" && val.type != ValueType::BOOLEAN) {
        throw std::runtime_error("Runtime error: BOOLEAN declaration needs BOOLEAN value: " + node->name);
    }
    varTable.declareVar(node->name, val, node->isConst);
}

void Interpreter::executeAssign(AstNode* node) {
    if (node->children.empty()) {
        throw std::runtime_error("Missing value in assignment");
    }
    Value val = evaluate(node->children[0]);
    varTable.setVar(node->name, val);
}

void Interpreter::executeIncDec(AstNode* node) {
    Var& var = varTable.getVar(node->name);
    if (var.isConst) {
        throw std::runtime_error("Can't increment const value: " + node->name);
    }
    if (var.value.type != ValueType::UINT) {
        throw std::runtime_error("INC_DEC only accepts UINT");
    }
    if (node->op == "INC") var.value.uintValue++;
    else if (node->op == "DEC") {
        if (var.value.uintValue == 0) {
            throw std::runtime_error("Can't decrement 0 value: " + node->name);
        }
        var.value.uintValue--;
    }
    else {
        throw std::runtime_error("Unknown operation: " + node->op + " in " + node->name);
    }
}

void Interpreter::executeIf(AstNode* node) {
    if (node->children.size() < 2) {
        throw std::runtime_error("Incorrect if: " + node->name);
    }
    Value cond = evaluate(node->children[0]);
    checkBool(cond);
    if (cond.boolValue) execute(node->children[1]);
    else if (node->children.size() == 3) execute(node->children[2]);
}

void Interpreter::executeWhile(AstNode* node) {
    if (node->children.size() < 2) {
        throw std::runtime_error("Incorrect while: " + node->name);
    }
    while (true) {
        Value cond = evaluate(node->children[0]);
        checkBool(cond);
        if (!cond.boolValue) break;
        execute(node->children[1]);
    }
}

Value Interpreter::evaluateUnaryOp(AstNode *node) {
    if (node->children.empty()) throw std::runtime_error("Missing operand in unary operation");
        Value val = evaluate(node->children[0]);
        if (node->op == "NOT") {
            checkBool(val);
            return Value::makeBool(!val.boolValue);
        }
        throw std::runtime_error("Unknown operation: " + node->op + " in unary operation");
}


Value Interpreter::evaluateBinaryOp(AstNode* node) {
    if (node->children.size() < 2) throw std::runtime_error("Missing operands in binary operation");
    Value left = evaluate(node->children[0]);
    Value right = evaluate(node->children[1]);
    if (node->op == "OR") {
        checkBool(left);
        checkBool(right);
        return Value::makeBool(left.boolValue || right.boolValue);
    }
    else if (node->op == "GT") {
        checkUint(left);
        checkUint(right);
        return Value::makeBool(left.uintValue > right.uintValue);
    }
    else if (node->op == "LT") {
        checkUint(left);
        checkUint(right);
        return Value::makeBool(left.uintValue < right.uintValue);
    }
    throw std::runtime_error("Unknown operation: " + node->op + " binary operation");
}


