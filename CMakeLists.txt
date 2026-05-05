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

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        language
)
add_executable(automata3
        main.cpp
        language/AST.cpp
        ${BISON_PARSER_OUTPUTS}
        ${FLEX_LEXER_OUTPUTS}
)
