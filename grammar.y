%{

#include "class/ast.hpp"
#include "y.tab.h"
#include "lex.yy.h"

int yylex();

ExpressionList *root;

using namespace std;

%}

%token END_OF_FILE 0
%token LEFT_PAREN
%token RIGHT_PAREN

%token <int_val> VALUE_INTEGER
%token <double_val> VALUE_DOUBLE
%token <bool_val> VALUE_BOOL
%token <string_val> VALUE_STRING
%token <string_val> VALUE_SYMBOL

%union{
    std::vector<Expression*> *expressions;
    ExpressionList* expressionList;
    Expression* expression;
    Atom* atom;

    int int_val;
    double double_val;
    bool bool_val;
    const char* string_val;
}

%type <expressionList> Program
%type <expressions> ExpressionList List
%type <expression> Expression
%type <atom> Atom

%%
Program:
    ExpressionList                                  { root = new ExpressionList($1); }
    ;

ExpressionList:
    Expression                                      { $$ = new std::vector<Expression*>; $$->push_back($1); }
    | ExpressionList Expression                     { $$ = $1; $$->push_back($2); }
    ;

Expression:
    Atom                                            { $$ = new Expression($1); }
    | List                                          { $$ = new Expression($1); }
    ;

Atom:
    VALUE_SYMBOL                                    { $$ = new Symbol($1); }
    | VALUE_DOUBLE                                  { $$ = new Double($1); }
    | VALUE_BOOL                                    { $$ = new Bool($1); }
    | VALUE_STRING                                  { $$ = new String($1); }
    ;

List:
    LEFT_PAREN ExpressionList RIGHT_PAREN           { $$ = $2; }
    ;
%%


