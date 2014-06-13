#include <iostream>
#include "common.h"

extern NProgram* programBlock;
extern int yyparse();
extern FILE * yyin;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "could not open %s\n", argv[1]);
            exit(1);
        }
        yyin = file;
    } else {
        yyin = stdin;
    }

    yyparse();
    std::cout << programBlock << std::endl;
    return 0;
}
