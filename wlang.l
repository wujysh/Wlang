%{
#include <string>
#include "parser.hpp"
#define SAVE_TOKEN yylval.nstring = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() {}
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*
%option yylineno

%%
[ \n\t]                                 {}

if                                      { printf("%s", yytext); return TOKEN(KIF); }
then                                    { printf("%s", yytext); return TOKEN(KTHEN); }
else                                    { printf("%s", yytext); return TOKEN(KELSE); }
while                                   { printf("%s", yytext); return TOKEN(KWHILE); }
do                                      { printf("%s", yytext); return TOKEN(KDO); }
integer                                 { printf("%s", yytext); return TOKEN(KINTEGER); }
float                                   { printf("%s", yytext); return TOKEN(KFLOAT); }
string                                  { printf("%s", yytext); return TOKEN(KSTRING); }
input                                   { printf("%s", yytext); return TOKEN(KINPUT); }
output                                  { printf("%s", yytext); return TOKEN(KOUTPUT); }
end                                     { printf("%s", yytext); return TOKEN(KEND); }
def                                     { printf("%s", yytext); return TOKEN(KDEF); }
var                                     { printf("%s", yytext); return TOKEN(KVAR); }
as                                      { printf("%s", yytext); return TOKEN(KAS); }
begin                                   { printf("%s", yytext); return TOKEN(KBEGIN); }

and|"&&"                                { printf("%s", yytext); return TOKEN(KAND); }
or|"||"                                 { printf("%s", yytext); return TOKEN(KOR); }

{ID}                                    { printf("%s", yytext); SAVE_TOKEN; return TIDENTIFIER; }

{DIGIT}+                                { printf("%s", yytext); SAVE_TOKEN; return TINTEGER; }

{DIGIT}+"."{DIGIT}*                     { printf("%s", yytext); SAVE_TOKEN; return TFLOAT; }

"\""{STRING}"\""                        { printf("%s", yytext); SAVE_TOKEN; return TSTRING; }

"+"                                     { printf("%s", yytext); return TOKEN(TPLUS); }
"-"                                     { printf("%s", yytext); return TOKEN(TMINUS); }
"*"                                     { printf("%s", yytext); return TOKEN(TMULTIPLY); }
"/"                                     { printf("%s", yytext); return TOKEN(TDEVIDE); }
"="                                     { printf("%s", yytext); return TOKEN(TASSIGN); }

"<"                                     { printf("%s", yytext); return TOKEN(TLESS); }
"<="                                    { printf("%s", yytext); return TOKEN(TLESSEQUAL); }
">"                                     { printf("%s", yytext); return TOKEN(TGREATER); }
">="                                    { printf("%s", yytext); return TOKEN(TGREATEREQUAL); }
"<>"                                    { printf("%s", yytext); return TOKEN(TNOTEQUAL); }
"=="                                    { printf("%s", yytext); return TOKEN(TEQUAL); }

"{"                                     { printf("%s", yytext); return TOKEN(TLEFTBRACE); }
"}"                                     { printf("%s", yytext); return TOKEN(TRIGHTBRACE); }
"("                                     { printf("%s", yytext); return TOKEN(TLEFTBRACKET); }
")"                                     { printf("%s", yytext); return TOKEN(TRIGHTBRACKET); }
";"                                     { printf("%s", yytext); return TOKEN(TSEMICOLON); }
","                                     { printf("%s", yytext); return TOKEN(TCOMMA); }
":"                                     { printf("%s", yytext); return TOKEN(TCOLON); }
.                                       { printf("Unknown token!\n"); yyterminate(); }

%%
/*
int main(int argc, char** argv) {
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
    return 0;
}
*/
