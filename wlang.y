%{
   class NProgram;
   NProgram *programBlock; /* the top level root node of our final AST */
   extern int yylex();
   extern void yyerror(char const *);
   extern int yyleng;
%}

%define parse.error verbose
%locations

%code requires {
   #include "ast.h"
   #define SETLOC() setLocation(yylloc.first_line, yylloc.first_column, yyleng)
}

/* Represents the many different ways we can access our data */
%union {
    class Node *node;
    class NExpression *expression;
    class NStatement *statement;
    class NFunction *function;
    class NProgram *program;
    class NBinaryOperator *binary_operator;
    class NIdentifier *identifier;
    class NArgument *argument;
    class NInteger *ninteger;
    class NFloat *nfloat;
    class NMethodCall *method_call;
    StatementList *statement_vector;
    ArgumentList *argument_vector;
    FunctionList *function_vector;
    ExpressionList *expression_vector;
    IdentifierList *identifier_vector;
    std::string *nstring;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <nstring> TIDENTIFIER "IDENTIFIER" TINTEGER "INTEGER" TFLOAT "FLOAT" TSTRING "STRING"
%token <token> VAR IF THEN ELSE WHILE DO INPUT OUTPUT FUNCTION DEF AS RETURN INTEGER FLOAT STRING VOID
%token <token> AND "&&" OR "||" KBEGIN "BEGIN" KEND "END"
%token <token> TPLUS "+" TMINUS "-" TMULTIPLY "*" TDIVIDE "/" TASSIGN "="
%token <token> TLESS "<" TLESSEQUAL "<=" TGREATER ">" TGREATEREQUAL ">=" TNOTEQUAL "<>" TEQUAL "=="
%token <token> TLEFTBRACE "{" TRIGHTBRACE "}" TLEFTBRACKET "(" TRIGHTBRACKET ")" 
%token <token> TSEMICOLON ";" TCOMMA "," TCOLON ":"
%token END 0 "end of file"

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
//%type <expression> expression boolexpression
%type <statement> statement defstatement ifstatement whilestatement inputstatement outputstatement assignstatement returnstatement exprstatement
%type <function> function
%type <argument_vector> arguments
%type <statement_vector> statementblock statements
%type <function_vector> functions
%type <expression_vector> expressions
%type <identifier_vector> identifiers
%type <token> datatype relation
%type <binary_operator> expression boolexpression term factor boolterm boolfactor
%type <method_call> methodcall
%type <program> program
%type <identifier> identifier;
%type <argument> argument;

%start program

%%

program : functions { programBlock = new NProgram(*$1); $$->SETLOC(); }
        | error END {}
        ;

functions : function { $$ = new FunctionList(); $$->push_back($1); }
          | functions function { $$->push_back($2); }
          ;

function : DEF identifier TLEFTBRACKET arguments TRIGHTBRACKET TCOLON datatype statementblock KEND { $$ = new NFunction(*$2, *$4, $7, *$8); $$->SETLOC(); }
         | DEF identifier TLEFTBRACKET arguments error TRIGHTBRACKET TCOLON datatype statementblock KEND {}
         | error KEND {}
         ;

arguments : %empty { $$ = new ArgumentList(); }
          | argument { $$ = new ArgumentList(); $$->push_back($1); }
          | arguments TCOMMA argument { $1->push_back($3); }
          ;

argument : identifier TCOLON datatype { $$ = new NArgument(*$1, $3); $$->SETLOC(); }
         ;

statementblock : %empty { $$ = new StatementList(); }
               | statements { $$ = $1; }
               | KBEGIN statements { $$ = $2; }
               ;

statements : statement { $$ = new StatementList(); $$->push_back($1); }
           | statements statement { $$->push_back($2); }
           ;

statement : ifstatement | assignstatement | whilestatement | inputstatement | outputstatement | defstatement | returnstatement | exprstatement
          | error TSEMICOLON {}
          | error KEND {}
          ;

exprstatement : expressions TSEMICOLON { $$ = new NExprStatement(*$1); $$->SETLOC(); }
              ;

defstatement : VAR identifiers TCOLON datatype TSEMICOLON { $$ = new NDefStatement($4, *$2); $$->SETLOC(); }
//             | VAR identifiers TCOLON datatype TASSIGN expression TSEMICOLON { $$ = new NDefStatement($4, *$2, *$6); }
             ;

identifiers : identifier { $$ = new IdentifierList(); $$->push_back($1); }
            | identifiers TCOMMA identifier { $1->push_back($3); }
            ;

identifier : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; $$->SETLOC(); }
           ;

datatype : INTEGER | FLOAT | STRING | VOID
         ;

inputstatement : INPUT identifiers TSEMICOLON { $$ = new NInputStatement(*$2); $$->SETLOC(); }
               ;

outputstatement : OUTPUT expressions TSEMICOLON { $$ = new NOutputStatement(*$2); $$->SETLOC(); }
                ;

returnstatement : RETURN TSEMICOLON { $$ = new NReturnStatement(); $$->SETLOC(); }
                | RETURN expression TSEMICOLON { $$ = new NReturnStatement(*$2); $$->SETLOC(); }
                ;

expressions : expression { $$ = new ExpressionList(); $$->push_back($1); }
            | boolexpression { $$ = new ExpressionList(); $$->push_back($1); }
            | expressions TCOMMA expression { $1->push_back($3); }
            | expressions TCOMMA boolexpression { $1->push_back($3); }
            ;

assignstatement : identifier TASSIGN expression TSEMICOLON { $$ = new NAssignStatement(*$1, *$3); $$->SETLOC(); }
                ;

ifstatement : IF boolexpression THEN statementblock KEND { $$ = new NIfStatement(*$2, *$4); $$->SETLOC(); }
            | IF boolexpression error THEN statementblock KEND {}
            | IF boolexpression THEN statementblock ELSE statementblock KEND { $$ = new NIfStatement(*$2, *$4, *$6); $$->SETLOC(); }
            | IF boolexpression error THEN statementblock ELSE statementblock KEND {}
            ;

whilestatement : WHILE boolexpression DO statementblock KEND { $$ = new NWhileStatement(*$2, *$4); $$->SETLOC(); }
               | WHILE boolexpression error DO statementblock KEND {}
               ;

expression : term { $$ = $1; }
           | expression TPLUS term { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
           | expression TMINUS term { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
           ;

term : factor { $$ = $1; }
     | term TMULTIPLY factor { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
     | term TDIVIDE factor { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
     ;

factor : identifier { $<identifier>$ = $1; }
       | TINTEGER { $<ninteger>$ = new NInteger(atol($1->c_str())); delete $1; $$->SETLOC(); }
       | TFLOAT { $<nfloat>$ = new NFloat(atof($1->c_str())); delete $1; $$->SETLOC(); }
       | methodcall { $<method_call>$ = $1; }
       | TLEFTBRACKET expression TRIGHTBRACKET { $$ = $2; }
       ;

methodcall : identifier TLEFTBRACKET expressions TRIGHTBRACKET { $$ = new NMethodCall(*$1, *$3); $$->SETLOC(); }
           | identifier TLEFTBRACKET TRIGHTBRACKET { $$ = new NMethodCall(*$1); $$->SETLOC(); }
           ;

boolexpression : boolterm { $$ = $1; }
               | boolexpression OR boolterm { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
               ;

boolterm : boolfactor { $$ = $1; }
         | boolterm AND boolfactor { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
         ;

boolfactor : expression relation expression { $$ = new NBinaryOperator(*$1, $2, *$3); $$->SETLOC(); }
           | TLEFTBRACKET boolexpression TRIGHTBRACKET { $$ = $2; }
           ;

relation : TLESS | TLESSEQUAL | TGREATER | TGREATEREQUAL | TEQUAL | TNOTEQUAL
         ;

%%
