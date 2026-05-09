%option noyywrap
%{
#include "parser.tab.h"
#include <cstdlib>
#include <cstring>
void yyerror(const char *);
%}

%%
"UINT" return UINT;
"BOOLEAN" return BOOLEAN;
"CUINT" return CUINT;
"CBOOLEAN" return CBOOLEAN;
"TRUE" return TRUE;
"FALSE" return FALSE;
"INC" return INC;
"DEC" return DEC;
"GT" return GT;
"LT" return LT;
"OR" return OR;
"NOT" return NOT;
"FORW" return FORW;
"BACK" return BACK;
"LEFT" return LEFT;
"RIGHT" return RIGHT;
"GETF" return GETF;
"GETB" return GETB;
"GETR" return GETR;
"GETL" return GETL;
"IF" return IF;
"ELSE" return ELSE;
"WHILE" return WHILE;
"DO" return DO;
"FUNCTION" return FUNCTION;

[0-9]+ {
    yylval.intValue = static_cast<unsigned int>(std::strtoul(yytext, nullptr, 10));
    return NUMBER;
}
[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.strValue = strdup(yytext);
    return NAME;
}

"=" return '=';
"(" return '(';
")" return ')';
"{" return '{';
"}" return '}';
"[" return '[';
"]" return ']';
"," return ',';

\n return NEWLINE;
[ \t\r]+ ;
. {
    yyerror("Unknown character");
    return yytext[0];
}
%%