#ifndef AUTOMATA3_VALUE_H
#define AUTOMATA3_VALUE_H

enum class ValueType {
    NONE,
    UINT,
    BOOLEAN
};

struct Value {
    ValueType type = ValueType::NONE;
    unsigned int uintValue = 0;
    bool boolValue = false;
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
};

#endif //AUTOMATA3_VALUE_H