#ifndef AUTOMATA3_VALUE_H
#define AUTOMATA3_VALUE_H

enum class ValueType {
    NONE,
    UINT,
    BOOLEAN,
    ARR1UINT,
    ARR1BOOL,
    ARR2BOOL,
    ARR2UINT
};

struct Value {
    ValueType type = ValueType::NONE;
    unsigned int uintValue = 0;
    bool boolValue = false;
    std::vector<Value> arr1Values;
    std::vector<std::vector<Value>> arr2Rows;
    static Value makeUint(unsigned int value) {
        Value res;
        res.type = ValueType::UINT;
        res.uintValue = value;
        return res;
    }
    static Value makeBool(bool value) {
        Value res;
        res.type = ValueType::BOOLEAN;
        res.boolValue = value;
        return res;
    }
    static Value makeNone() {
        return Value{};
    }
    static Value makeArr1(ValueType type, const std::vector<Value>& values) {
        Value res;
        res.type = type;
        res.arr1Values = values;
        return res;
    }
    static Value makeArr2(ValueType type, const std::vector<std::vector<Value>>& values) {
        Value res;
        res.type = type;
        res.arr2Rows = values;
        return res;
    }
};

#endif //AUTOMATA3_VALUE_H