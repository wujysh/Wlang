%{
#include <string>
#include "parser.hpp"
#define SAVE_TOKEN yylval.nstring = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() {}
extern void yyerror(char const *);
int lineno = 1, tokenpos = 0;
char linebuf[500];
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*

%%

\n.*    { strcpy(linebuf, yytext+1);
          lineno++;
          tokenpos = 0;
          yyless(1);  
        }
[\t]    { tokenpos += 8; }
[ ]     { tokenpos += 1; }

if                                      { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KIF); }
then                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KTHEN); }
else                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KELSE); }
while                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KWHILE); }
do                                      { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KDO); }
integer                                 { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KINTEGER); }
float                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KFLOAT); }
string                                  { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KSTRING); }
input                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KINPUT); }
output                                  { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KOUTPUT); }
end                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KEND); }
def                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KDEF); }
var                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KVAR); }
as                                      { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KAS); }
begin                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KBEGIN); }

and|"&&"                                { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KAND); }
or|"||"                                 { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KOR); }

{ID}                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TIDENTIFIER; }

{DIGIT}+                                { tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TINTEGER; }

{DIGIT}+"."{DIGIT}*                     { tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TFLOAT; }

"\""{STRING}"\""                        { tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TSTRING; }

"+"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TPLUS); }
"-"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TMINUS); }
"*"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TMULTIPLY); }
"/"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TDEVIDE); }
"="                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TASSIGN); }

"<"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TLESS); }
"<="                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TLESSEQUAL); }
">"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TGREATER); }
">="                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TGREATEREQUAL); }
"<>"                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TNOTEQUAL); }
"=="                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TEQUAL); }

"{"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TLEFTBRACE); }
"}"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TRIGHTBRACE); }
"("                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TLEFTBRACKET); }
")"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TRIGHTBRACKET); }
";"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TSEMICOLON); }
","                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TCOMMA); }
":"                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(TCOLON); }

.                                       { tokenpos += yyleng; yyerror("error: invalid character"); }

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
