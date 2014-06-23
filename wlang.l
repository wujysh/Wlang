%{
    #include <string>
    #include "parser.hpp"

    #define SAVE_TOKEN yylval.nstring = new std::string(yytext, yyleng)
    #define TOKEN(t) (yylval.token = t)

    extern "C" int yywrap() {}
    extern void yyerror(char const *);
    extern int yynerrs;

    int lineno = 1, tokenpos = 0;
    char linebuf[500];
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*

%%

\n.*                                    { strcpy(linebuf, yytext+1);
                                          lineno++;
                                          tokenpos = 0;
                                          yyless(1);  
                                        }
[\t]                                    { tokenpos += 8; }
[ ]                                     { tokenpos += 1; }

if                                      { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(IF); }
then                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(THEN); }
else                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(ELSE); }
while                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(WHILE); }
do                                      { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(DO); }
integer                                 { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(INTEGER); }
float                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(FLOAT); }
string                                  { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(STRING); }
input                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(INPUT); }
output                                  { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(OUTPUT); }
def                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(DEF); }
var                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(VAR); }
as                                      { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(AS); }
return                                  { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(RETURN); }
begin                                   { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KBEGIN); }
end                                     { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(KEND); }

and|"&&"                                { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(AND); }
or|"||"                                 { tokenpos += yyleng; /*printf("%s", yytext);*/ return TOKEN(OR); }

{ID}                                    { tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TIDENTIFIER; }

(\+|\-)?{DIGIT}+                        { tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TINTEGER; }

(\+|\-)?({DIGIT}+|{DIGIT}+\.{DIGIT}*|{DIGIT}*\.{DIGIT}+)([eE][\+-]?{DIGIT}+)?  { 
                                          tokenpos += yyleng; /*printf("%s", yytext);*/ SAVE_TOKEN; return TFLOAT; }

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

.                                       { tokenpos += yyleng; yynerrs++; yyerror("lexical error: invalid character"); }

%%