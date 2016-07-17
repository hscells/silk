#include <stdio.h>
#include "y.tab.h"
#include "class/ast.hpp"
#include "class/runtime.hpp"
#include "grammar.tab.h"
#include "lex.yy.h"

extern ExpressionList* root;
Runtime* runtime;

int main(int argc, char* argv[]) {

    yyin = fopen(argv[1], "r");

    yyparse();

    runtime = new Runtime();

    new LambdaFunction();
    new AddFunction();
    new PrintlnFunction();
    new SubtractFunction();
    new DefFunction();
    new ListFunction();
    new FirstFunction();
    new RestFunction();

    if (root) {
        root->eval();
    }


    return 0;
}