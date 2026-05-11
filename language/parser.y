%code requires {
#include "AST.h"
}
%define parse.error detailed
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
%token PUSHF
%token PUSHB
%token PUSHR
%token PUSHL
%token UNDO
%token NEWLINE

%token <intValue> NUMBER
%token <strValue> NAME
%token IF
%token WHILE
%token ELSE
%token DO
%token FUNCTION
%token ARR1BOOL
%token ARR2BOOL
%token ARR2UINT
%token ARR1UINT
%token EXTEND1
%token EXTEND2
%token SIZE1
%token SIZE2


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
%type <node> arrDecl
%type <node> arrSet
%type <node> arrExtend
%type <node> arrSize
%type <node> arr1Values
%type <node> arr2Rows

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
    | funcDecl { $$ = $1; }
    | funcCall { $$ = $1; }
    | funcCallAssign { $$ = $1; }
    | arrDecl { $$ = $1; }
    | arrSet { $$ = $1; }
    | arrExtend { $$ = $1; }
    ;

block:
    '{' lines '}' { $$ = $2; }
    ;

optionalNewlines:
    /* */ { $$ = nullptr; }
    | optionalNewlines NEWLINE { $$ = nullptr; }
    ;

ifExpr:
    IF '(' expr ')' optionalNewlines block {
        $$ = makeIf($3, $6, nullptr);
    }
    | IF '(' expr ')' optionalNewlines block ELSE optionalNewlines block {
        $$ = makeIf($3, $6, $9);
    }
    ;

whileExpr:
    WHILE '(' expr ')' DO block {
        $$ = makeWhile($3, $6);
    }
    ;

funcDecl:
    FUNCTION NAME '(' paramOpt ')' block {
        $$ = makeFuncDecl($2, createBlock(), $4, $6);
        free($2);
    }
    | NAME '=' expr FUNCTION NAME '(' paramOpt ')' block {
        AstNode* list = appendNode(createBlock(), makeFuncReturn($1, $3));
        $$ = makeFuncDecl($5, list, $7, $9);
        free($1);
        free($5);
    }
    | '[' returnList ']' FUNCTION NAME '(' paramOpt ')' block {
        $$ = makeFuncDecl($5, $2, $7, $9);
        free($5);
    }
    ;

returnList:
    returnSingle {
        $$ = appendNode(createBlock(), $1);
    }
    | returnList ',' returnSingle {
        $$ = appendNode($1, $3);
    }
    ;

returnSingle:
    NAME '=' expr {
        $$ = makeFuncReturn($1, $3);
        free($1);
    }
    ;

paramOpt:
    /* */ { $$ = createBlock(); }
    | paramList { $$ = $1; }
    ;
paramList:
    paramSingle { $$ = appendNode(createBlock(), $1); }
    | paramList ',' paramSingle { $$ = appendNode($1, $3); }
    ;

paramSingle:
    NAME '=' expr {
        $$ = makeFuncParam($1, $3);
        free($1);
    }
    ;

funcCall:
    NAME '(' argOpt ')' {
        $$ = makeFuncCall($1, $3);
        free($1);
    }
    ;

funcCallAssign:
    '[' targetList ']' '=' NAME '(' argOpt ')' {
        $$ = makeFuncCallAssign($5, $2, $7);
        free($5);
    }
    ;

argOpt:
    /* */ { $$ = createBlock(); }
    | argList { $$ = $1; }
    ;
argList:
    argSingle {
        $$ = appendNode(createBlock(), $1);
    }
    | argList ',' argSingle { $$ = appendNode($1, $3); }
    | argList ',' { $$ = appendNode($1, makeFuncEmpty()); }
    | ',' argSingle {
        $$ = appendNode(createBlock(), makeFuncEmpty());
        $$ = appendNode($$, $2);
    }
    ;
argSingle:
    expr { $$ = $1; }
    ;

targetList:
    targetSingle { $$ = appendNode(createBlock(), $1); }
    | targetList ',' targetSingle { $$ = appendNode($1, $3); }
    | targetList ',' { $$ = appendNode($1, makeFuncEmpty()); }
    | ',' targetSingle {
        $$ = appendNode(createBlock(), makeFuncEmpty());
        $$ = appendNode($$, $2);
    }
    ;
targetSingle:
    NAME {
        $$ = makeVar($1);
        free($1);
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
    | funcCall {
        $$ = $1;
    }
    | INC NAME {
        $$ = makeIncDec("INC", $2);
        free($2);
    }
    | DEC NAME {
        $$ = makeIncDec("DEC", $2);
        free($2);
    }
    | arrSize { $$ = $1; }
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
    | PUSHF { $$ = makeRobotCmd("PUSHF"); }
    | PUSHB { $$ = makeRobotCmd("PUSHB"); }
    | PUSHR { $$ = makeRobotCmd("PUSHR"); }
    | PUSHL { $$ = makeRobotCmd("PUSHL"); }
    | UNDO { $$ = makeRobotCmd("UNDO"); }
    ;

arrDecl:
    ARR1UINT NAME '=' '[' arr1Values ']' {
        $$ = makeArrayDeclaration("ARR1UINT", $2, $5);
        free($2);
    }
    | ARR1BOOL NAME '=' '[' arr1Values ']' {
        $$ = makeArrayDeclaration("ARR1BOOL", $2, $5);
        free($2);
    }
    | ARR2UINT NAME '=' '[' arr2Rows ']' {
        $$ = makeArrayDeclaration("ARR2UINT", $2, $5);
        free($2);
    }
    | ARR2BOOL NAME '=' '[' arr2Rows ']' {
        $$ = makeArrayDeclaration("ARR2BOOL", $2, $5);
        free($2);
    }
    ;

arr1Values:
    expr { $$ = appendNode(createBlock(), $1); }
    | arr1Values ',' expr { $$ = appendNode($1, $3); }
    ;
arr2Rows:
    arr1Values { $$ = appendNode(createBlock(), $1); }
    | arr2Rows ';' arr1Values { $$ = appendNode($1, $3); }
    ;

arrSet:
    NAME '(' expr ')' '=' expr {
        AstNode* indices = appendNode(createBlock(), $3);
        $$ = makeArraySet($1, indices, $6);
        free($1);
    }
    | NAME '(' expr ',' expr ')' '=' expr {
        AstNode* indices = appendNode(createBlock(), $3);
        indices = appendNode(indices, $5);
        $$ = makeArraySet($1, indices, $8);
        free($1);
    }
    ;

arrExtend:
    EXTEND1 NAME expr {
        $$ = makeArrayExtend("EXTEND1", $2, $3, nullptr);
        free($2);
    }
    | EXTEND2 NAME expr expr {
        $$ = makeArrayExtend("EXTEND2", $2, $3, $4);
        free($2);
    }
    ;

arrSize:
    SIZE1 NAME {
        $$ = makeArraySize("SIZE1", $2, nullptr);
        free($2);
    }
    | SIZE2 NAME expr {
        $$ = makeArraySize("SIZE2", $2, $3);
        free($2);
    }
    ;

%%

void yyerror(const char* err){
    std::cerr << "Parse error: " << err << std::endl;
}