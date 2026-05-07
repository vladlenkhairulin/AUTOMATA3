
#ifndef AUTOMATA3_VARTABLE_H
#define AUTOMATA3_VARTABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Value.h"

struct Var {
    Value value;
    bool isConst = false;
};

class VarTable {
private:
    std::vector<std::unordered_map<std::string, Var>> scoping;
public:
    VarTable() {
        pushScope();
    }
    void printVarTable() const;
    void pushScope();
    void popScope();

    void declareVar(const std::string& name, const Value& value, bool isConst);
    void setVar(const std::string& name, const Value& value);
    Var& getVar(const std::string& name);

};


#endif //AUTOMATA3_VARTABLE_H