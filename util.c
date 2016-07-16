#include <stdio.h>
#include "y.tab.h"

void yyerror(const char *s) {
    fprintf(stderr, "Parse Error:\n%s\n", s);
}