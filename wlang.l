%{
#include <stdio.h>
int line = 0, offset = 0;
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*

%%
[ \n\t]+                                                     {line++; offset = 0;}

if|else|while|do|integer|float|string|input|output|function|end|def|as|begin        {printf("（5，%s）\n", yytext);}

and|or                                                      {printf("（8，%s）\n", yytext);}

{ID}                                                        {printf("（1，%s）\n", yytext);}

{DIGIT}+                                                    {printf("（2，%s）\n", yytext);}

{DIGIT}+"."{DIGIT}*                                         {printf("（3，%s）\n", yytext);}

"\""{STRING}"\""                                            {printf("（4，%s）\n", yytext);}

"+"|"-"|"*"|"/"|"="                                         {printf("（6，%s）\n", yytext);}

"<"|"<="|">"|">="|"<>"|"=="                                 {printf("（7，%s）\n", yytext);}

"{"|"}"|"("|")"|";"|","                                     {printf("（9，%s）\n", yytext);}

.                                                           {printf("ERROR: %s\n", yytext);}

%%

int yywrap() {
    return 1;
}

void main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }
    yylex();
}
