#include <iostream>
#include "ast.h"
#include "codegen.h"

extern NProgram* programBlock;
extern int yyparse(), yyleng, lineno, tokenpos;
extern FILE* yyin;
extern char linebuf[500];
void yyerror(char const *);
void printAST();
char filename[500];

int main(int argc, char **argv) {
    strcpy(filename, argv[1]);
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

    fgets(linebuf, 500, yyin);
    if (linebuf[strlen(linebuf)-1] == '\n')
        linebuf[strlen(linebuf)-1] = '\0';
    rewind(yyin);

    yyparse();

    printAST();

    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();

    return 0;
}

void yyerror(char const *s) {
    printf("%s:", filename);
    printf("%d:%d: %s:\n%s\n", lineno, tokenpos-yyleng+1, s, linebuf);
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
//            NDefStatement defstatement = *((NDefStatement*)*stmtIter);
//            IdentifierList identifiers = defstatement.identifiers;
//            IdentifierList::iterator identIter;
//            for (identIter = identifiers.begin(); identIter != identifiers.end(); identIter++) {
//                identifier = **identIter;
//                cout << identifier.name << " ";
//            }
//            cout << defstatement.type << endl;
        }
    }
    cout << endl;
}
