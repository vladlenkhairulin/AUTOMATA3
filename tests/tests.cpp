#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "Value.h"
#include "VarTable.h"
#include "Robot.h"
#include "Interpreter.h"
#include "AST.h"
#include <fstream>

TEST_CASE("Value") {
    Value a = Value::makeUint(10);
    REQUIRE(a.type == ValueType::UINT);
    REQUIRE(a.uintValue == 10);
    Value b = Value::makeBool(true);
    REQUIRE(b.type == ValueType::BOOLEAN);
    REQUIRE(b.boolValue == true);
    Value n = Value::makeNone();
    REQUIRE(n.type == ValueType::NONE);
    std::vector<Value> arr1 = {Value::makeUint(1), Value::makeUint(2)};
    Value v1 = Value::makeArr1(ValueType::ARR1UINT, arr1);
    REQUIRE(v1.type == ValueType::ARR1UINT);
    REQUIRE(v1.arr1Values.size() == 2);
    REQUIRE(v1.arr1Values[1].type == ValueType::UINT);
    REQUIRE(v1.arr2Rows.empty());

    std::vector<std::vector<Value>> arr2 = {
        {Value::makeBool(true), Value::makeBool(false)}
    };
    Value v2 = Value::makeArr2(ValueType::ARR2BOOL, arr2);
    REQUIRE(v2.type == ValueType::ARR2BOOL);
    REQUIRE(v2.arr2Rows.size() == 1);
    REQUIRE(v2.arr2Rows[0][0].boolValue == true);
    REQUIRE(v2.arr2Rows[0][1].boolValue == false);
    REQUIRE(v2.arr1Values.empty());
}

TEST_CASE("VarTable") {
    VarTable table;
    table.declareVar("x", Value::makeUint(5), false);
    REQUIRE(table.getVar("x").value.type == ValueType::UINT);
    REQUIRE(table.getVar("x").value.uintValue == 5);
    table.setVar("x", Value::makeUint(10));
    REQUIRE(table.getVar("x").value.uintValue == 10);

    table.declareVar("const", Value::makeUint(5), true);
    REQUIRE_THROWS(table.setVar("const", Value::makeUint(10)));
    REQUIRE_THROWS(table.setVar("x", Value::makeBool(true)));

    REQUIRE_THROWS(table.declareVar("x", Value::makeUint(3), true));
    REQUIRE_THROWS(table.getVar("g"));

}

TEST_CASE("Scopes") {
    VarTable table;
    table.declareVar("x", Value::makeUint(5), false);
    table.pushScope();
    table.declareVar("x", Value::makeUint(2), false);
    REQUIRE(table.getVar("x").value.uintValue == 2);
    table.declareVar("y", Value::makeUint(3), false);
    table.popScope();
    REQUIRE(table.getVar("x").value.uintValue == 5);
    REQUIRE_THROWS(table.getVar("y"));
}

TEST_CASE("AST") {
    AstNode* block = createBlock();
    AstNode* dec1 = makeDeclaration("UINT", "x", makeNumber(5), false);
    AstNode* assign1 = makeAssign("x", makeNumber(10));
    AstNode* cond = makeBinaryOperation("GT", makeVar("x"), makeNumber(0));
    AstNode* then = appendNode(createBlock(), makeIncDec("DEC", "x"));
    AstNode* ifNode = makeIf(cond, then, nullptr);
    appendNode(block, dec1);
    appendNode(block, assign1);
    appendNode(block, ifNode);
    AstNode* root = createProgram(block);
    REQUIRE(root->nodeType == NodeType::PROGRAM);
    REQUIRE(root->children.size() == 1);
    REQUIRE(block->nodeType == NodeType::BLOCK);
    REQUIRE(block->children.size() == 3);
    REQUIRE(dec1->nodeType == NodeType::DECLARATION);
    REQUIRE(dec1->dataType == "UINT");
    REQUIRE(assign1->nodeType == NodeType::ASSIGN);
    REQUIRE(assign1->name == "x");
    REQUIRE(ifNode->nodeType == NodeType::IF);
    REQUIRE(ifNode->children.size() == 2);
    deleteAst(root);

}

TEST_CASE("AST_FUNCTION") {
    AstNode* returns = appendNode(createBlock(), makeFuncReturn("res", makeNumber(0)));
    AstNode* params = appendNode(createBlock(), makeFuncParam("a", makeNumber(3)));
    AstNode* body = appendNode(createBlock(), makeAssign("res", makeVar("a")));
    AstNode* func = makeFuncDecl("id", returns, params, body);
    REQUIRE(func->nodeType == NodeType::FUNC_DECL);
    REQUIRE(func->name == "id");
    REQUIRE(func->children.size() == 3);
    REQUIRE(func->children[1] == params);
    REQUIRE(returns->children[0]->nodeType == NodeType::FUNC_RETURN);
    REQUIRE(params->children[0]->nodeType == NodeType::FUNC_PARAM);
    deleteAst(func);

}

TEST_CASE("Interpret run1") {
    AstNode* block = createBlock();
    appendNode(block, makeDeclaration("UINT", "x", makeNumber(3), false));
    appendNode(block, makeDeclaration("BOOLEAN", "ok", makeBool(true), false));
    AstNode* whileBody = appendNode(createBlock(), makeIncDec("DEC", "x"));
    AstNode* whileCond = makeBinaryOperation("GT", makeVar("x"), makeNumber(0));
    appendNode(block, makeWhile(whileCond, whileBody));
    AstNode* ifBody = appendNode(createBlock(), makeAssign("ok", makeBool(false)));
    appendNode(block, makeIf(makeVar("ok"), ifBody, nullptr));
    appendNode(block, makeDeclaration("BOOLEAN", "hm", makeBinaryOperation("OR", makeBool(false), makeUnaryOperation("NOT", makeBool(false))), false));
    AstNode* root = createProgram(block);
    Interpreter interpreter(nullptr);
    REQUIRE_NOTHROW(interpreter.run(root));
    deleteAst(root);
}

TEST_CASE("Interpreter errors") {
    SECTION("Types mismatch") {
        AstNode* block1 = createBlock();
        appendNode(block1, makeDeclaration("UINT", "x", makeBool(true), false));
        AstNode* root1 = createProgram(block1);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root1));
        deleteAst(root1);
    }
    SECTION("Decrementing boolean") {
        AstNode* block2 = createBlock();
        appendNode(block2, makeDeclaration("BOOLEAN", "ok", makeBool(true), false));
        appendNode(block2, makeIncDec("DEC", "ok"));
        AstNode* root2 = createProgram(block2);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root2));
        deleteAst(root2);
    }
    SECTION("Incrementing const value") {
        AstNode* block3 = createBlock();
        appendNode(block3, makeDeclaration("UINT", "x", makeNumber(3), true));
        appendNode(block3, makeIncDec("INC", "x"));
        AstNode* root3 = createProgram(block3);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root3));
        deleteAst(root3);
    }
}

TEST_CASE("Interpreter functions") {
    AstNode* block = createBlock();
    AstNode* returns = appendNode(createBlock(), makeFuncReturn("res", makeNumber(1)));
    AstNode* params = appendNode(createBlock(), makeFuncParam("a", makeNumber(0)));
    AstNode* body = appendNode(createBlock(), makeAssign("res", makeVar("a")));
    appendNode(block, makeFuncDecl("test", returns, params, body));
    appendNode(block, makeDeclaration("UINT", "x", makeNumber(0), false));
    AstNode* args = appendNode(createBlock(), makeNumber(7));
    appendNode(block, makeAssign("x", makeFuncCall("test", args)));
    AstNode* root = createProgram(block);
    Interpreter interpreter(nullptr);
    REQUIRE_NOTHROW(interpreter.run(root));
    deleteAst(root);
}

TEST_CASE("Void function") {
    AstNode* block = createBlock();
    appendNode(block, makeDeclaration("BOOLEAN", "x", makeBool(false), false));
    AstNode* body = appendNode(createBlock(), makeAssign("x", makeUnaryOperation("NOT", makeVar("X"""))));
    appendNode(block, makeFuncDecl("inversion", createBlock(), createBlock(), body));
    appendNode(block, makeFuncCall("inversion", createBlock()));
    AstNode* root = createProgram(block);
    Interpreter interpreter(nullptr);
    REQUIRE_NOTHROW(interpreter.run(root));
    deleteAst(root);
}

TEST_CASE("Function errors") {
    SECTION("More args than params") {
        AstNode* block = createBlock();
        AstNode* returns = appendNode(createBlock(), makeFuncReturn("res", makeNumber(0)));
        AstNode* params = appendNode(createBlock(), makeFuncParam("a", makeNumber(1)));
        AstNode* body = appendNode(createBlock(), makeAssign("res", makeVar("a")));
        appendNode(block, makeFuncDecl("f", returns, params, body));
        appendNode(block, makeDeclaration("UINT", "x", makeNumber(0), false));
        AstNode* args = createBlock();
        appendNode(args, makeNumber(1));
        appendNode(args, makeNumber(2));
        appendNode(block, makeAssign("x", makeFuncCall("f", args)));
        AstNode* root = createProgram(block);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root));
        deleteAst(root);
    }
    SECTION("Function redeclaration") {
        AstNode* block = createBlock();
        appendNode(block, makeFuncDecl("f", createBlock(), createBlock(), createBlock()));
        appendNode(block, makeFuncDecl("f", createBlock(), createBlock(), createBlock()));
        AstNode* root = createProgram(block);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root));
        deleteAst(root);
    }
    SECTION("Assigning void function") {
        AstNode* block2 = createBlock();
        appendNode(block2, makeFuncDecl("empty", createBlock(), createBlock(), createBlock()));
        appendNode(block2, makeDeclaration("UINT", "x", makeNumber(0), false));
        appendNode(block2, makeAssign("x", makeFuncCall("empty", createBlock())));
        AstNode* root2 = createProgram(block2);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root2));
        deleteAst(root2);
    }
}

TEST_CASE("Interpreter arrays") {
    AstNode* block = createBlock();
    AstNode* nums = createBlock();
    appendNode(nums, makeNumber(1));
    appendNode(nums, makeNumber(2));
    appendNode(block, makeArrayDeclaration("ARR1UINT", "arr1", nums));
    AstNode* index1Set = appendNode(createBlock(), makeNumber(1));
    appendNode(block, makeArraySet("arr1", index1Set, makeNumber(3)));
    AstNode* index1Get = appendNode(createBlock(), makeNumber(1));
    appendNode(block, makeDeclaration("UINT", "a", makeFuncCall("arr1", index1Get), false));
    AstNode* row1 = appendNode(createBlock(), makeNumber(2));
    appendNode(row1, makeNumber(7));
    AstNode* row2 = appendNode(createBlock(), makeNumber(2));
    appendNode(row2, makeNumber(3));
    AstNode* rows = appendNode(createBlock(), row1);
    appendNode(rows, row2);
    appendNode(block, makeArrayDeclaration("ARR2UINT", "arr2", rows));
    appendNode(block, makeArrayExtend("EXTEND1", "arr2", makeNumber(3), nullptr));

    appendNode(block, makeArrayExtend("EXTEND2", "arr2", makeNumber(2), makeNumber(5)));
    AstNode* index2 = appendNode(createBlock(), makeNumber(2));
    appendNode(index2, makeNumber(3));
    appendNode(block, makeArraySet("arr2", index2, makeNumber(48)));

    appendNode(block, makeDeclaration("UINT", "size", makeArraySize("SIZE1", "arr2", nullptr), true));
    appendNode(block, makeDeclaration("UINT", "row2Size", makeArraySize("SIZE2", "arr2", makeNumber(2)), false));

    AstNode* bools = createBlock();
    appendNode(bools, makeBool(true));
    appendNode(bools, makeBool(false));
    appendNode(block, makeArrayDeclaration("ARR1BOOL", "flags", bools));
    AstNode* boolIndex = appendNode(createBlock(), makeNumber(1));
    appendNode(block, makeArraySet("flags", boolIndex, makeBool(true)));
    AstNode* boolGetIndex = appendNode(createBlock(), makeNumber(1));
    appendNode(block, makeDeclaration("BOOLEAN", "flag", makeFuncCall("flags", boolGetIndex), false));

    AstNode* root = createProgram(block);
    Interpreter interpreter(nullptr);
    REQUIRE_NOTHROW(interpreter.run(root));
    REQUIRE(interpreter.getVar("a").value.uintValue == 3);
    REQUIRE(interpreter.getVar("size").value.uintValue == 3);
    REQUIRE(interpreter.getVar("row2Size").value.uintValue == 5);
    Var arr2 = interpreter.getVar("arr2");
    REQUIRE(arr2.value.type == ValueType::ARR2UINT);
    REQUIRE(arr2.value.arr2Rows.size() == 3);
    REQUIRE(arr2.value.arr2Rows[2][3].uintValue == 48);
    REQUIRE(interpreter.getVar("flag").value.boolValue == true);
    deleteAst(root);
}

TEST_CASE("Interpreter empty function args") {
    AstNode* block = createBlock();
    AstNode* returns = createBlock();
    appendNode(returns, makeFuncReturn("a", makeNumber(0)));
    appendNode(returns, makeFuncReturn("b", makeNumber(0)));
    appendNode(returns, makeFuncReturn("c", makeNumber(0)));
    AstNode* params = createBlock();
    appendNode(params, makeFuncParam("x", makeNumber(1)));
    appendNode(params, makeFuncParam("y", makeNumber(2)));
    appendNode(params, makeFuncParam("z", makeNumber(3)));
    AstNode* body = createBlock();
    appendNode(body, makeAssign("a", makeVar("x")));
    appendNode(body, makeAssign("b", makeVar("y")));
    appendNode(body, makeAssign("c", makeVar("z")));

    appendNode(block, makeFuncDecl("test", returns, params, body));
    appendNode(block, makeDeclaration("UINT", "p", makeNumber(0), false));
    appendNode(block, makeDeclaration("UINT", "r", makeNumber(0), false));
    AstNode* targets = createBlock();
    appendNode(targets, makeVar("p"));
    appendNode(targets, makeFuncEmpty());
    appendNode(targets, makeVar("r"));
    AstNode* args = createBlock();
    appendNode(args, makeNumber(10));
    appendNode(args, makeFuncEmpty());
    appendNode(args, makeNumber(30));
    appendNode(block, makeFuncCallAssign("test", targets, args));
    AstNode* root = createProgram(block);
    Interpreter interpreter(nullptr);
    REQUIRE_NOTHROW(interpreter.run(root));
    deleteAst(root);
}

TEST_CASE("Array errors") {
    SECTION("Empty indices") {
        AstNode* block1 = createBlock();
        AstNode* values = appendNode(createBlock(), makeNumber(1));
        appendNode(block1, makeArrayDeclaration("ARR1UINT", "nums", values));
        appendNode(block1, makeArraySet("nums", createBlock(), makeNumber(5)));
        AstNode* root = createProgram(block1);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root));
        deleteAst(root);
    }
    SECTION("Extend-shrink") {
        AstNode* block = createBlock();
        AstNode* values = appendNode(createBlock(), makeNumber(1));
        appendNode(block, makeArrayDeclaration("ARR1UINT", "nums", values));
        appendNode(block, makeArrayExtend("EXTEND1", "nums", makeNumber(0), nullptr));
        AstNode* root = createProgram(block);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root));
        deleteAst(root);
    }
    SECTION("SIZE2 out of bounds") {
        AstNode* block = createBlock();
        AstNode* row = appendNode(createBlock(), makeNumber(1));
        AstNode* rows = appendNode(createBlock(), row);
        appendNode(block, makeArrayDeclaration("ARR2UINT", "m", rows));
        appendNode(block, makeDeclaration("UINT", "bad", makeArraySize("SIZE2", "m", makeNumber(5)), false));
        AstNode* root = createProgram(block);
        Interpreter interpreter(nullptr);
        REQUIRE_THROWS(interpreter.run(root));
        deleteAst(root);
    }
}

TEST_CASE("Robot move") {
    std::ofstream file("test_move.txt");
    file << "SIZE 3 1\n";
    file << "ROBOT 0 0 E\n";
    file << "EXIT 2 0\n";
    file.close();
    Robot robot;
    REQUIRE(robot.loadGameInfo("test_move.txt"));
    REQUIRE(robot.getDist("GETF") == 2);
    REQUIRE(robot.move("FORW"));
    REQUIRE(robot.getDist("GETF") == 1);
    REQUIRE(robot.move("FORW"));
    REQUIRE(robot.getDist("GETF") == 0);
    REQUIRE_FALSE(robot.move("FORW"));

}

TEST_CASE("Robot push undo") {
    SECTION("Cannot undo twice") {
        std::ofstream file("test_undo.txt");
        file << "SIZE 4 1\n";
        file << "ROBOT 0 0 E\n";
        file << "EXIT 3 0\n";
        file << "WALL 1 0\n";
        file.close();
        Robot robot;
        REQUIRE(robot.loadGameInfo("test_undo.txt"));
        REQUIRE_FALSE(robot.move("FORW"));
        REQUIRE(robot.pushWall("PUSHF"));
        REQUIRE(robot.undo());
        REQUIRE_FALSE(robot.undo());
    }
    SECTION("Robot and wall can't be on the same tile") {
        std::ofstream file("test_undo.txt");
        file << "SIZE 4 1\n";
        file << "ROBOT 0 0 E\n";
        file << "EXIT 3 0\n";
        file << "WALL 1 0\n";
        file.close();
        Robot robot;
        REQUIRE(robot.loadGameInfo("test_undo.txt"));
        REQUIRE_FALSE(robot.move("FORW"));
        REQUIRE(robot.pushWall("PUSHF"));
        REQUIRE(robot.move("FORW"));
        REQUIRE_FALSE(robot.undo());
    }
}

TEST_CASE("Robot loading errors") {
    SECTION("Wall and exit can't be on the same tile") {
        std::ofstream file("test_load_error.txt");
        file << "SIZE 4 1\n";
        file << "ROBOT 0 0 E\n";
        file << "EXIT 3 0\n";
        file << "WALL 3 0\n";
        file.close();
        Robot robot;
        REQUIRE_FALSE(robot.loadGameInfo("test_load_error.txt"));
    }
    SECTION("Robot cannot spawn on exit tile") {
        std::ofstream file("test_spawn.txt");
        file << "SIZE 4 1\n";
        file << "ROBOT 0 0 E\n";
        file << "EXIT 0 0\n";
        file.close();
        Robot robot;
        REQUIRE_FALSE(robot.loadGameInfo("test_spawn.txt"));
    }
    SECTION("Robot coordinates out of borders") {
        std::ofstream file("test_coordinates.txt");
        file << "SIZE 4 1\n";
        file << "ROBOT 10 0 E\n";
        file << "EXIT 3 0\n";
        file.close();
        Robot robot;
        REQUIRE_FALSE(robot.loadGameInfo("test_coordinates.txt"));
    }
}

TEST_CASE("ROBOT AND INTERPRET") {
    std::ofstream file("test_interpret.txt");
    file << "SIZE 4 1\n";
    file << "ROBOT 0 0 E\n";
    file << "EXIT 3 0\n";
    file << "WALL 1 0\n";
    file.close();
    Robot robot;
    REQUIRE(robot.loadGameInfo("test_interpret.txt"));
    AstNode* block = createBlock();
    appendNode(block, makeDeclaration("UINT", "dist", makeLocateCmd("GETF"), false));
    appendNode(block, makeDeclaration("BOOLEAN", "pushed", makeRobotCmd("PUSHF"), false));
    appendNode(block, makeDeclaration("BOOLEAN", "undo", makeRobotCmd("UNDO"), false));
    appendNode(block, makeDeclaration("BOOLEAN", "moved", makeRobotCmd("RIGHT"), false));
    AstNode* root = createProgram(block);
    Interpreter interpreter(&robot);
    REQUIRE_NOTHROW(interpreter.run(root));
    deleteAst(root);
}
