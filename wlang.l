%{
    #include <string>
    #include <vector>
    #include "parser.hpp"

    #define SAVE_TOKEN yylval.nstring = new std::string(yytext, yyleng)
    #define TOKEN(t) (yylval.token = t)

    extern "C" int yywrap() {}
    extern void yyerror(char const *);
    extern int yynerrs;
    extern vector<string> buf;

    /* handle locations */
    int yyline = 1, yycolumn = 0;
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yyline; yylloc.first_column = yycolumn+1; \
                           yylloc.last_column = yycolumn+yyleng; yycolumn += yyleng;
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
STRING   ([^\"\\]?(\\.)?(\\\n)?)*

%%

\n.*                                    { buf.push_back(yytext+1);  // save the next line
                                          yyline++;
                                          yycolumn = 0;
                                          yyless(1);  
                                        }
[\t]                                    { yycolumn += 7; }
[ ]                                     {}

if                                      { return TOKEN(IF); }
then                                    { return TOKEN(THEN); }
else                                    { return TOKEN(ELSE); }
while                                   { return TOKEN(WHILE); }
do                                      { return TOKEN(DO); }
integer                                 { return TOKEN(INTEGER); }
float                                   { return TOKEN(FLOAT); }
string                                  { return TOKEN(STRING); }
input                                   { return TOKEN(INPUT); }
output                                  { return TOKEN(OUTPUT); }
def                                     { return TOKEN(DEF); }
var                                     { return TOKEN(VAR); }
as                                      { return TOKEN(AS); }
return                                  { return TOKEN(RETURN); }
begin                                   { return TOKEN(KBEGIN); }
end                                     { return TOKEN(KEND); }

and|"&&"                                { return TOKEN(AND); }
or|"||"                                 { return TOKEN(OR); }

{ID}                                    { SAVE_TOKEN; return TIDENTIFIER; }

(\+|\-)?{DIGIT}+                        { SAVE_TOKEN; return TINTEGER; }

(\+|\-)?({DIGIT}+|{DIGIT}+\.{DIGIT}*|{DIGIT}*\.{DIGIT}+)([eE][\+-]?{DIGIT}+)?  { SAVE_TOKEN; return TFLOAT; }

"\""{STRING}"\""                        { SAVE_TOKEN; return TSTRING; }

"+"                                     { return TOKEN(TPLUS); }
"-"                                     { return TOKEN(TMINUS); }
"*"                                     { return TOKEN(TMULTIPLY); }
"/"                                     { return TOKEN(TDIVIDE); }
"="                                     { return TOKEN(TASSIGN); }

"<"                                     { return TOKEN(TLESS); }
"<="                                    { return TOKEN(TLESSEQUAL); }
">"                                     { return TOKEN(TGREATER); }
">="                                    { return TOKEN(TGREATEREQUAL); }
"<>"                                    { return TOKEN(TNOTEQUAL); }
"=="                                    { return TOKEN(TEQUAL); }

"{"                                     { return TOKEN(TLEFTBRACE); }
"}"                                     { return TOKEN(TRIGHTBRACE); }
"("                                     { return TOKEN(TLEFTBRACKET); }
")"                                     { return TOKEN(TRIGHTBRACKET); }
";"                                     { return TOKEN(TSEMICOLON); }
","                                     { return TOKEN(TCOMMA); }
":"                                     { return TOKEN(TCOLON); }

.                                       { yynerrs++; yyerror("lexical error: invalid character"); }

%%