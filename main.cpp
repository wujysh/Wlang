#include <iostream>
#include "ast.h"
#include "codegen.h"

extern NProgram* programBlock;
extern int yyparse(), yyleng, lineno, tokenpos, yynerrs;
extern FILE* yyin;
extern char linebuf[500];
extern void createCoreFunctions(CodeGenContext& context);

void terminateCompile();
void yyerror(char const *);
void printAST();
char filename[500];

int main(int argc, char **argv) {
    if (argc > 1) {
        // open the file
        strcpy(filename, argv[1]);
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "wlang: fatal error: %s: No such file or directory\n", argv[1]);
            exit(1);
        }
        yyin = file;

    } else {
        // use standard input
        strcpy(filename, "wlang");
        yyin = stdin;
    }

    // buffer the first line
    fgets(linebuf, 500, yyin);
    if (linebuf[strlen(linebuf)-1] == '\n')
        linebuf[strlen(linebuf)-1] = '\0';
    rewind(yyin);

    if (yyparse() == 1 || yynerrs > 0) {
        terminateCompile();
    } else {
        printAST();

        CodeGenContext context;
        //createCoreFunctions(context);
        context.generateCode(*programBlock);
        context.runCode();
    }

    return 0;
}

void terminateCompile() {
    fprintf(stderr, "%d error generated.\n", yynerrs);
    fprintf(stderr, "compilation terminated.\n");
    exit(1);
}

void yyerror(char const *s) {
    printf("%s:%d:%d: %s:\n%s\n", filename, lineno, tokenpos-yyleng+1, s, linebuf);
    printf("%*s", tokenpos-yyleng, "");
    for (int i = 0; i < yyleng; i++) {
      printf("%c", '^');
    }
    printf("\n");
}

void printAST() {
    cout << endl;
    cout << "┌ Program: " << programBlock << endl;
    FunctionList::iterator funcIter;
    FunctionList functions = programBlock->functions;
    for (funcIter = functions.begin(); funcIter != functions.end(); funcIter++) {
        NIdentifier identifier = (*funcIter)->id;
        cout << "│ ┝ Function: " << identifier.name << " " << *funcIter << endl;

        StatementList statements = (*funcIter)->block;
        StatementList::iterator stmtIter;
        for (stmtIter = statements.begin(); stmtIter != statements.end(); stmtIter++) {
            cout << "│ │ ┝ Statement: " << *stmtIter << " ";

            //cout << "- DefStatement ";
            cout << typeid(**stmtIter).name() << endl;

            // if (typeid(**stmtIter).name() == typeid(NDefStatement).name()) {
            //     NDefStatement defstatement = *((NDefStatement*)*stmtIter);
            //     IdentifierList identifiers = defstatement.identifiers;
            //     IdentifierList::iterator identIter;
            //     for (identIter = identifiers.begin(); identIter != identifiers.end(); identIter++) {
            //         identifier = **identIter;
            //         cout << identifier.name << " ";
            //     }
            //     cout << defstatement.type << endl;
            // }
            if (typeid(**stmtIter).name() == typeid(NIfStatement).name()) {
                NIfStatement ifstatement = *((NIfStatement*)*stmtIter);
                StatementList thenblock = ifstatement.thenblock, elseblock = ifstatement.elseblock;
                StatementList::iterator stmtIter_if;

                for (stmtIter_if = thenblock.begin(); stmtIter_if != thenblock.end(); stmtIter_if++) {
                    cout << "│ │ │ ┝ Then Statement: " << *stmtIter_if << " ";
                    cout << typeid(**stmtIter_if).name() << endl;
                }
                for (stmtIter_if = elseblock.begin(); stmtIter_if != elseblock.end(); stmtIter_if++) {
                    cout << "│ │ │ ┝ Else Statement: " << *stmtIter_if << " ";
                    cout << typeid(**stmtIter_if).name() << endl;
                }

                //cout << &thenblock << " " << &elseblock << endl;
            }
        }
    }
    cout << endl;
}
