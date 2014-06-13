%{
#include <string>
#include <wlang.hpp>
#include "common.h"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() {}

%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*
%option yylineno

%%
[ \n\t]                                 {}

if                                      return TOKEN(KIF);
else                                    return TOKEN(KELSE);
while                                   return TOKEN(KWHILE);
do                                      return TOKEN(KDO);
integer                                 return TOKEN(KINTEGER);
float                                   return TOKEN(KFLOAT);
string                                  return TOKEN(KSTRING);
input                                   return TOKEN(KINPUT);
output                                  return TOKEN(KOUTPUT);
function                                return TOKEN(KFUNCTION);
end                                     return TOKEN(KEND);
def                                     return TOKEN(KDEF);
as                                      return TOKEN(KAS);
begin                                   return TOKEN(KBEGIN);

and                                     return TOKEN(KAND);
or                                      return TOKEN(KOR);

{ID}                                    SAVE_TOKEN; return TIDENTIFIER;

{DIGIT}+                                SAVE_TOKEN; return TINTEGER;

{DIGIT}+"."{DIGIT}*                     SAVE_TOKEN; return TFLOAT;

"\""{STRING}"\""                        SAVE_TOKEN; return TSTRING;

"+"                                     return TOKEN(TPLUS);
"-"                                     return TOKEN(TMINUS);
"*"                                     return TOKEN(TMULTIPLY);
"/"                                     return TOKEN(TDEVIDE);
"="                                     return TOKEN(TASSIGN);

"<"                                     return TOKEN(TLESS);
"<="                                    return TOKEN(TLESSEQUAL);
">"                                     return TOKEN(TGREATER);
">="                                    return TOKEN(TGREATEREQUAL);
"<>"                                    return TOKEN(TNOTEQUAL);
"=="                                    return TOKEN(TEQUAL);

"{"                                     return TOKEN(TLEFTBRACE);
"}"                                     return TOKEN(TRIGHTBRACE);
"("                                     return TOKEN(TLEFTBRACLET);
")"                                     return TOKEN(TRIGHTBRACKET);
";"                                     return TOKEN(TSEMICOLON);
","                                     return TOKEN(TCOMMA);

.                                       return TOKEN(TERROR);

%%

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
