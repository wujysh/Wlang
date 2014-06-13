#include <iostream>
#include "common.h"

extern NProgram* programBlock;
extern int yyparse();

int main(int argc, char **argv) {
    yyparse();
    std::cout << programBlock << std::endl;
    return 0;
}
