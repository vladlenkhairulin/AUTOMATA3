
#include "VarTable.h"
#include <iostream>
#include <stdexcept>

void printValue(const Value& value) {
    if (value.type == ValueType::UINT) {
        std::cout << value.uintValue;
    }
    else if (value.type == ValueType::BOOLEAN) {
        std::cout << (value.boolValue ? "TRUE" : "FALSE");
    }
}

void VarTable::pushScope() {
    scoping.push_back({});
}
void VarTable::popScope() {
    if (scoping.empty()) {
        throw std::runtime_error("Runtime error: no scopes currently");
    }
    scoping.pop_back();
}

void VarTable::declareVar(const std::string& name, const Value& value, bool isConst) {
    if (scoping.empty()) pushScope();
    auto& curScope = scoping.back();
    if (curScope.contains(name)) {
        throw std::runtime_error("Runtime error: already declared variable: " + name);
    }
    Var var;
    var.value = value;
    var.isConst = isConst;
    curScope[name] = var;
}

void VarTable::setVar(const std::string& name, const Value& value) {
    for (int i = static_cast<int>(scoping.size()) - 1; i >= 0; i--) {
        auto it = scoping[i].find(name);
        if (it != scoping[i].end()) {
            if (it->second.isConst) {
                throw std::runtime_error("Runtime error: cannot change const variable: " + name);
            }
            if (it->second.value.type != value.type) {
                throw std::runtime_error("Runtime error: types conflict in assignment: " + name);
            }
            it->second.value = value;
            return;
        }
    }
    throw std::runtime_error("Runtime error: no such variable: " + name);
}

Var& VarTable::getVar(const std::string& name) {
    for (int i = static_cast<int>(scoping.size()) - 1; i >= 0; i--) {
        auto it = scoping[i].find(name);
        if (it != scoping[i].end()) {
            return it->second;
        }
    }
    throw std::runtime_error("Runtime error: unknown variable: " + name);
}

void VarTable::printVarTable() const {
    std::cout << "\nRuntime Variables:\n";
    if (scoping.empty()) return;
    for (const auto& pair : scoping.front()) {
        if (pair.second.isConst) std::cout << "const ";
        std::cout << pair.first << " = ";
        if (pair.second.value.type == ValueType::UINT) std::cout << pair.second.value.uintValue;
        else if (pair.second.value.type == ValueType::BOOLEAN) std::cout << (pair.second.value.boolValue? "TRUE" : "FALSE");
        else if (pair.second.value.type == ValueType::ARR1UINT || pair.second.value.type == ValueType::ARR1BOOL) {
            for (Value val : pair.second.value.arr1Values) {
                printValue(val);
                std::cout << " ";
            }
        }
        else if (pair.second.value.type == ValueType::ARR2UINT || pair.second.value.type == ValueType::ARR2BOOL) {
            for (std::vector row : pair.second.value.arr2Rows) {
                for (Value val : row) {
                    printValue(val);
                    std::cout << " ";
                }
                std::cout << "; ";
            }
        }
        else throw std::runtime_error("Runtime error: variable has NONE value: " + pair.first);
        std::cout << std::endl;
    }
}
