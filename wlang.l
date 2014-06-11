%{
#include "common.h"
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*
%option yylineno

%%
[ \n\t]+                                                     {}

if|else|while|do|integer|float|string|input|output|function|end|def|as|begin        {printf("（5，%s）\n", yytext);}

and|or                                                      {printf("（8，%s）\n", yytext);}

{ID}                                                        {printf("（1，%s）\n", yytext);}

{DIGIT}+                                                    {printf("（2，%s）\n", yytext);}

{DIGIT}+"."{DIGIT}*                                         {printf("（3，%s）\n", yytext);}

"\""{STRING}"\""                                            {printf("（4，%s）\n", yytext);}

"+"|"-"|"*"|"/"|"="                                         {printf("（6，%s）\n", yytext);}

"<"|"<="|">"|">="|"<>"|"=="                                 {printf("（7，%s）\n", yytext);}

"{"|"}"|"("|")"|";"|","                                     {printf("（9，%s）\n", yytext);}

.                                                           {printf("ERROR: line %d: %s\n", yylineno, yytext);}

%%

struct Token getToken() {
    struct Token token;
    token.type = yylex();
    strcpy(token.sVal, yytext);
    return token;
}

void main(int argc, char** argv) {
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
    yylex();
}

int yywrap() {
    return 1;
}
