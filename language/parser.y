%code requires {
#include "AST.h"
}

%{
#include <iostream>
#include <cstdlib>
#include "AST.h"
int yylex();
void yyerror(const char* err);
static AstNode* root = nullptr;
AstNode* getRoot() {
    return root;
}
%}

%union {
    unsigned int intValue;
    char* strValue;
    AstNode* node;
}

%token UINT
%token BOOLEAN
%token CUINT
%token CBOOLEAN
%token TRUE
%token FALSE
%token INC
%token DEC
%token GT
%token LT
%token OR
%token NOT

%token FORW
%token BACK
%token LEFT
%token RIGHT
%token GETF
%token GETB
%token GETR
%token GETL
%token NEWLINE

%token <intValue> NUMBER
%token <strValue> NAME
%token IF
%token WHILE
%token ELSE
%token DO
%token FUNCTION

%type <node> block
%type <node> ifExpr
%type <node> whileExpr
%type <node> optionalNewlines

%type <node> program
%type <node> lines
%type <node> line
%type <node> statement
%type <node> declar
%type <node> const_declar
%type <node> assign
%type <node> inc_dec
%type <node> expr
%type <node> robot_cmd
%type <node> locate_cmd

%type <node> funcDecl
%type <node> funcCall
%type <node> funcCallAssign
%type <node> returnOpt
%type <node> returnList
%type <node> returnSingle
%type <node> paramOpt
%type <node> paramList
%type <node> paramSingle
%type <node> argOpt
%type <node> argList
%type <node> argSingle
%type <node> targetList
%type <node> targetSingle



%left OR
%nonassoc GT LT
%right NOT

%%
program:
    lines {
        root = createProgram($1);
        $$ = root;
    }
    ;
lines:
    /* */ {
        $$ = createBlock();
    }
    | lines line {
        $$ = appendNode($1, $2);
    }
    ;
line:
    NEWLINE { $$ = nullptr; }
    | statement NEWLINE { $$ = $1; }
    | statement { $$ = $1; }
    ;
statement:
    declar { $$ = $1; }
    | const_declar { $$ = $1; }
    | assign { $$ = $1; }
    | inc_dec { $$ = $1; }
    | robot_cmd { $$ = $1; }
    | ifExpr { $$ = $1; }
    | whileExpr { $$ = $1; }
    ;

block:
    '{' lines '}' { $$ = $2; }
    ;

optionalNewlines:
    /* */ { $$ = nullptr; }
    | optionalNewlines NEWLINE { $$ = nullptr; }
    ;

ifExpr:
    IF '(' expr ')' block {
        $$ = makeIf($3, $5, nullptr);
    }
    | IF '(' expr ')' block optionalNewlines ELSE block {
        $$ = makeIf($3, $5, $8);
    }
    ;

whileExpr:
    WHILE '(' expr ')' DO block {
        $$ = makeWhile($3, $6);
    }
    ;
declar:
    UINT NAME '=' expr {
        /* std::cout << "declare UINT " << $2 <<std::endl; */
        $$ = makeDeclaration("UINT", $2, $4, false);
        free($2);
    }
    | BOOLEAN NAME '=' expr {
        /* std::cout << "declare BOOLEAN " << $2 << std::endl; */
        $$ = makeDeclaration("BOOLEAN", $2, $4, false);
        free($2);
    }
    ;
const_declar:
    CUINT NAME '=' expr {
        /*std::cout << "declare CUINT " << $2 <<std::endl;*/
        $$ = makeDeclaration("UINT", $2, $4, true);
        free($2);
    }
    | CBOOLEAN NAME '=' expr {
        /*std::cout << "declare CBOOLEAN " << $2 << std::endl;*/
        $$ = makeDeclaration("BOOLEAN", $2, $4, true);
        free($2);
    }
    ;
assign:
    NAME '=' expr {
        $$ = makeAssign($1, $3);
        /*std::cout << "assign arith expr to " << $1 << std::endl;*/
        free($1);
    }
    ;

inc_dec:
    INC NAME {
        /*std::cout << "INC " << $2 << std::endl;*/
        $$ = makeIncDec("INC", $2);
        free($2);
    }
    | DEC NAME {
        $$ = makeIncDec("DEC", $2);
        /*std::cout << "DEC " << $2 << std::endl;*/
        free($2);
    }
    ;

expr:
    NUMBER {
        $$ = makeNumber($1);
    }
    | NAME {
        $$ = makeVar($1);
        free($1);
    }
    | TRUE {
        $$ = makeBool(true);
    }
    | FALSE {
        $$ = makeBool(false);
    }
    | NOT expr {
        $$ = makeUnaryOperation("NOT", $2);
    }
    | expr OR expr {
        $$ = makeBinaryOperation("OR", $1, $3);
    }
    | expr GT expr {
        $$ = makeBinaryOperation("GT", $1, $3);
    }
    | expr LT expr {
        $$ = makeBinaryOperation("LT", $1, $3);
    }
    | locate_cmd {
        $$ = $1;
    }
    | robot_cmd {
        $$ = $1;
    }
    ;

locate_cmd:
    GETF {
        $$ = makeLocateCmd("GETF");
        /*std::cout << "GETF " << std::endl;*/
        }
    | GETB {
        $$ = makeLocateCmd("GETB");
        /*std::cout << "GETB " << std::endl;*/
    }
    | GETR {
        $$ = makeLocateCmd("GETR");
        /*std::cout << "GETR " << std::endl;*/
    }
    | GETL {
        $$ = makeLocateCmd("GETL");
        /*std::cout << "GETL " << std::endl;*/
    }
    ;

robot_cmd:
    FORW{
        $$ = makeRobotCmd("FORW");
        /*std::cout << "FORW " << std::endl;*/
    }
    | BACK {
        $$ = makeRobotCmd("BACK");
        /*std::cout << "BACK " << std::endl;*/
    }
    | LEFT {
        $$ = makeRobotCmd("LEFT");
        /*std::cout << "LEFT " << std::endl;*/
    }
    | RIGHT {
        $$ = makeRobotCmd("RIGHT");
        /*std::cout << "RIGHT " << std::endl;*/
    }
    ;

%%

void yyerror(const char* err){
    std::cerr << "Parse error: " << err << std::endl;
}