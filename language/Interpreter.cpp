
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
        case NodeType::FUNC_CALL:
            executeFuncCall(node);
            break;
        case NodeType::FUNC_CALL_ASSIGN:
            executeFuncCallAssign(node);
            break;
        case NodeType::FUNC_DECL:
            executeFuncDeclaration(node);
            break;
        case NodeType::ROBOT_CMD:
            evaluate(node);
            break;
        case NodeType::ARR_DECL:
            executeArrDeclaration(node);
            break;
        case NodeType::ARR_SET:
            executeArrSet(node);
            break;
        case NodeType::ARR_EXTEND:
            executeArrExtend(node);
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
        case NodeType::FUNC_CALL:
            return evaluateFuncCall(node);
        case NodeType::INC_DEC:
            return evaluateIncDec(node);
        case NodeType::ROBOT_CMD:
            if (game == nullptr) {
                throw std::runtime_error("Runtime error: no game");
            }
            if (node->op == "BACK" || node->op == "FORW" || node->op == "RIGHT" ||  node->op == "LEFT") {
                return Value::makeBool(game->move(node->op));
            }
            if (node->op == "PUSHF" || node->op == "PUSHB" || node->op == "PUSHL" || node->op == "PUSHR") {
                return Value::makeBool(game->pushWall(node->op));
            }
            if (node->op == "UNDO") {
                return Value::makeBool(game->undo());
            }
            throw std::runtime_error("Unknown ROBOT command type");
        case NodeType::LOCATE_CMD:
            if (game == nullptr) {
                throw std::runtime_error("Runtime error: no game");
            }
            return Value::makeUint(game->getDist(node->op));
        case NodeType::ARR_SIZE:
            return evaluateArrSize(node);
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
    if (node->children.size() != 2 && node->children.size() != 3) {
        throw std::runtime_error("Incorrect if: " + node->name);
    }
    Value cond = evaluate(node->children[0]);
    checkBool(cond);
    if (cond.boolValue) execute(node->children[1]);
    else if (node->children.size() == 3) execute(node->children[2]);
}

void Interpreter::executeWhile(AstNode* node) {
    if (node->children.size() != 2) {
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

void Interpreter::executeFuncDeclaration(AstNode* node) {
    if (functions.contains(node->name)) {
        throw std::runtime_error("Function " + node->name + " already exists");

    }
    if (node->children.size() != 3) {
        throw std::runtime_error("Function " + node->name + " is declared incorrectly");
    }
    functions[node->name] = node;
}

void Interpreter::executeFuncCall(AstNode* node) {
    if (node->children.empty()) throw std::runtime_error("Missing args in function call: " + node->name);
    callFunction(node->name, node->children[0]);
}

void Interpreter::executeFuncCallAssign(AstNode* node) {
    if (node->children.size() != 2) throw std::runtime_error("Function " + node->name + " is call-assigned incorrectly");
    AstNode* targets = node->children[0];
    AstNode* args = node->children[1];
    std::vector<Value> returns = callFunction(node->name, args);
    for (size_t i = 0; i < targets->children.size(); i++) {
        AstNode* target = targets->children[i];
        if (target->nodeType == NodeType::FUNC_EMPTY) continue;
        if (i >= returns.size()) {
            throw std::runtime_error("Function " + node->name + " has not enough return value");
        }
        varTable.setVar(target->name, returns[i]);
    }
}

Value Interpreter::evaluateFuncCall(AstNode* node) {
    if (node->children.empty()) {
        throw std::runtime_error("Missing args in function call: " + node->name);
    }
    if (functions.contains(node->name)) {
        std::vector<Value> returns = callFunction(node->name, node->children[0]);
        if (returns.empty()) {
            throw std::runtime_error("Missing return value: " + node->name);
        }
        return returns[0];
    }
    return evaluateArrGet(node);
}

std::vector<Value> Interpreter::evaluateArgs(AstNode* argsBlock) {
    std::vector<Value> args;
    if (argsBlock == nullptr) return args;
    for (AstNode* child : argsBlock->children) {
        if (child->nodeType == NodeType::FUNC_EMPTY) {
            args.push_back(Value::makeNone());
        }
        else args.push_back(evaluate(child));
    }
    return args;
}

Value Interpreter::evaluateIncDec(AstNode* node) {
    executeIncDec(node);
    return varTable.getVar(node->name).value;
}

std::vector<Value> Interpreter::callFunction(const std::string& name, AstNode* args) {
    auto it = functions.find(name);
    if (it == functions.end()) {
        throw std::runtime_error("Unknown function: " + name);
    }
    AstNode* func = it->second;
    AstNode* returns = func->children[0];
    AstNode* params = func->children[1];
    AstNode* body = func->children[2];
    std::vector<Value> argValues = evaluateArgs(args);
    varTable.pushScope();
    try {
        for (size_t i = 0; i < params->children.size(); i++) {
            AstNode* param = params->children[i];
            if (param->children.empty()) {
                throw std::runtime_error("Missing default value in parma: " + param->name);
            }
            Value defaultValue = evaluate(param->children[0]);
            Value resValue;
            if (i<argValues.size() && argValues[i].type != ValueType::NONE) {
                if (argValues[i].type != defaultValue.type) {
                    throw std::runtime_error("Value types mismatch " + param->name);
                }
                resValue = argValues[i];
            }
            else resValue = defaultValue;
            varTable.declareVar(param->name, resValue, false);
        }
        if (argValues.size() > params->children.size()) {
            throw std::runtime_error("Too many arguments in function: " + name);
        }
        for (AstNode* ret: returns->children) {
            if (ret->children.empty()) {
                throw std::runtime_error("Missing default return value: " + ret->name);
            }
            Value defaultValue = evaluate(ret->children[0]);
            varTable.declareVar(ret->name, defaultValue, false);
        }
        execute(body);
        std::vector<Value> returnValues;
        for (AstNode* ret : returns->children) {
            returnValues.push_back(varTable.getVar(ret->name).value);
        }
        varTable.popScope();
        return returnValues;
    }
    catch (...) {
        varTable.popScope();
        throw;
    }
}

unsigned int Interpreter::getIndex(AstNode* node) {
    Value value = evaluate(node);
    if (value.type != ValueType::UINT) {
        throw std::runtime_error("Array index not UINT " + node->name);
    }
    return value.uintValue;
}

Value Interpreter::getDefaultType(ValueType type){
    if (type == ValueType::ARR1UINT || type == ValueType::ARR2UINT) {
        return Value::makeUint(0);
    }
    return Value::makeBool(false);
}

void Interpreter::executeArrDeclaration(AstNode *node) {
    std::vector<Value> oneD;
    std::vector<std::vector<Value>> twoD;
    if (node->dataType == "ARR1UINT" || node->dataType == "ARR1BOOL") {
        ValueType elType = (node->dataType == "ARR1UINT")? ValueType::UINT : ValueType::BOOLEAN;
        for (AstNode* child : node->children[0]->children) {
            Value value = evaluate(child);
            if (value.type != elType) {
                throw std::runtime_error("Wrong type of element in array " + node->name);
            }
            oneD.push_back(value);
        }
        ValueType arrType = (node->dataType == "ARR1UINT")? ValueType::ARR1UINT : ValueType::ARR1BOOL;
        varTable.declareVar(node->name, Value::makeArr1(arrType, oneD), false);
        return;
    }
    else {
        ValueType elType = (node->dataType == "ARR2UINT")? ValueType::UINT : ValueType::BOOLEAN;
        for (AstNode* row : node->children[0]->children) {
            std::vector<Value> rowValues;
            for (AstNode* child : row->children) {
                Value value = evaluate(child);
                if (value.type != elType) {
                    throw std::runtime_error("Wrong type of element in array " + node->name);
                }
                rowValues.push_back(value);
            }
            twoD.push_back(rowValues);
        }
        ValueType arrType = (node->dataType == "ARR2UINT")? ValueType::ARR2UINT : ValueType::ARR2BOOL;
        varTable.declareVar(node->name, Value::makeArr2(arrType, twoD), false);
    }
}

void Interpreter::executeArrSet(AstNode* node) {
    Var& var = varTable.getVar(node->name);
    AstNode*  indices = node->children[0];
    Value value = evaluate(node->children[1]);
    if (indices->children.size() != 1 && indices->children.size() != 2) {
        throw std::runtime_error("Array SET needs 1 or 2 indexes: " + node->name);
    }

    if (indices->children.size() == 1) {
        if (var.value.type != ValueType::ARR1UINT && var.value.type != ValueType::ARR1BOOL) {
            throw std::runtime_error("Expected 1D array in SET: " + node->name);
        }
        unsigned int i = getIndex(indices->children[0]);
        if (i >= var.value.arr1Values.size()) {
            throw std::runtime_error("Array index out of bounds: " + node->name);
        }
        if (value.type != var.value.arr1Values[i].type) {
            throw std::runtime_error("Wrong element type in SET: " + node->name);
        }
        var.value.arr1Values[i] = value;
        return;
    }
    if (var.value.type != ValueType::ARR2UINT && var.value.type != ValueType::ARR2BOOL) {
        throw std::runtime_error("Expected 2D array in SET: " + node->name);
    }
    unsigned int i = getIndex(indices->children[0]);
    unsigned int j = getIndex(indices->children[1]);
    if (i >= var.value.arr2Rows.size() || j >= var.value.arr2Rows[i].size()) {
        throw std::runtime_error("Array index out of bounds: " + node->name);
    }
    if (value.type != var.value.arr2Rows[i][j].type) {
        throw std::runtime_error("Wrong element type in SET: " + node->name);
    }
    var.value.arr2Rows[i][j] = value;
}

void Interpreter::executeArrExtend(AstNode *node) {
    Var& var = varTable.getVar(node->name);
    if (var.value.type != ValueType::ARR1UINT && var.value.type != ValueType::ARR2UINT && var.value.type != ValueType::ARR1BOOL && var.value.type != ValueType::ARR2BOOL) {
        throw std::runtime_error("Wrong variable type in EXTEND: " + node->name);
    }
    if (node->op == "EXTEND1") {
        unsigned int newSize = getIndex(node->children[0]);
        if (var.value.type == ValueType::ARR1BOOL || var.value.type == ValueType::ARR1UINT) {
            if (newSize < var.value.arr1Values.size()) {
                throw std::runtime_error("Trying to decrease in EXTEND1: " + node->name);
            }
            var.value.arr1Values.resize(newSize, getDefaultType(var.value.type));
            return;
        }
        if (newSize < var.value.arr2Rows.size()) {
            throw std::runtime_error("Trying to decrease in EXTEND1: " + node->name);
        }
        size_t refSize = 0;
        if (!var.value.arr2Rows.empty()) {
            refSize = var.value.arr2Rows.back().size();
        }
        Value defValue = getDefaultType(var.value.type);
        while (var.value.arr2Rows.size() < newSize) {
            var.value.arr2Rows.push_back(std::vector<Value>(refSize, defValue));
        }
        return;
    }
    else {
        unsigned int rowIndex = getIndex(node->children[0]);
        unsigned int newSize = getIndex(node->children[1]);
        if (var.value.arr2Rows.size() <= rowIndex) {
            throw std::runtime_error("EXTEND2 out of range rowIndex: " + node->name);
        }
        if (var.value.arr2Rows[rowIndex].size() > newSize) {
            throw std::runtime_error("Trying to decrease in EXTEND2: " + node->name);
        }
        var.value.arr2Rows[rowIndex].resize(newSize, getDefaultType(var.value.type));
    }
}

Value Interpreter::evaluateArrSize(AstNode* node) {
    Var& var = varTable.getVar(node->name);
    if (node->op == "SIZE1") {
        if (var.value.type == ValueType::ARR1UINT || var.value.type == ValueType::ARR1BOOL) {
            return Value::makeUint(static_cast<unsigned int>(var.value.arr1Values.size()));
        }
        return Value::makeUint(static_cast<unsigned int>(var.value.arr2Rows.size()));
    }
    unsigned int i = getIndex(node->children[0]);
    if (i >= var.value.arr2Rows.size()) {
        throw std::runtime_error("SIZE2 out of bounds: " + node->name);
    }
    return Value::makeUint(static_cast<unsigned int>(var.value.arr2Rows[i].size()));
}

Value Interpreter::evaluateArrGet(AstNode* node) {
    Var& var = varTable.getVar(node->name);
    AstNode* args = node->children[0];
    if (args->children.size() == 1) {
        if (var.value.type != ValueType::ARR1UINT && var.value.type != ValueType::ARR1BOOL) {
            throw std::runtime_error("Expected 1D array: " + node->name);
        }
        unsigned int i = getIndex(args->children[0]);
        if (i >= var.value.arr1Values.size()) {
            throw std::runtime_error("GET index out of bounds: " + node->name);
        }
        return var.value.arr1Values[i];
    }
    if (var.value.type != ValueType::ARR2UINT && var.value.type != ValueType::ARR2BOOL) {
        throw std::runtime_error("Expected 2D array: " + node->name);
    }
    unsigned int i = getIndex(args->children[0]);
    unsigned int j = getIndex(args->children[1]);
    if (i >= var.value.arr2Rows.size() || j >= var.value.arr2Rows[i].size()) {
        throw std::runtime_error("GET 2D index out of bounds: " + node->name);
    }
    return var.value.arr2Rows[i][j];
}