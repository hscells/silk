%{

#include "class/ast.hpp"
#include "y.tab.h"
#include "grammar.tab.h"

%}

DIGIT [0-9]
CHAR [\+\-$?_a-zA-Z]

%option noyywrap

%%
"("                                 { return LEFT_PAREN; }
")"                                 { return RIGHT_PAREN; }
"true"                              { yylval.bool_val = true; return VALUE_BOOL; }
"false"                             { yylval.bool_val = false; return VALUE_BOOL; }

({DIGIT})?+(\.{DIGIT}+)?            { yylval.double_val = atof(yytext); return VALUE_DOUBLE; }

L?\"(\\.|[^\\"])*\"                 { yylval.string_val = strdup(yytext); return VALUE_STRING; }

{CHAR}({DIGIT}|{CHAR})*             { yylval.string_val = strdup(yytext); return VALUE_SYMBOL; }


[ \t\n\r]                               /* spaces and tabs, do nothing */
<<EOF>>                             { return END_OF_FILE; }
.                                   { yyerror("Unknown character"); }
%%