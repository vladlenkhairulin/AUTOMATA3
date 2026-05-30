cmake_minimum_required(VERSION 3.20)
project(automata3)

set(CMAKE_CXX_STANDARD 20)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(PARSER
        language/parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
)

FLEX_TARGET(LEXER
        language/lexer.l
        ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp
)

ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

add_library(game
        game/Robot.cpp
)

target_include_directories(game PUBLIC
        game
)

add_library(language
        language/AST.cpp
        language/VarTable.cpp
        language/Interpreter.cpp
        ${BISON_PARSER_OUTPUTS}
        ${FLEX_LEXER_OUTPUTS}
)
target_include_directories(language PUBLIC
        language
        game
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(language PUBLIC game)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        language
)
add_executable(automata3
        main.cpp
)

target_link_libraries(automata3 PRIVATE language game)

enable_testing()
add_subdirectory(tests)