%{
   class NProgram;
   NProgram *programBlock; /* the top level root node of our final AST */
   extern int yylex();
   void yyerror(char const *);

%}

%code requires {
   #include "common.h"
}

/* Represents the many different ways we can access our data */
%union {
    class Node *node;
    class NExpression *expression;
    class NStatement *statement;
    class NFunctionStatement *function_statement;
    class NProgram *program;
    class NBinaryOperator *binary_operator;
    class NIdentifier *identifier;
    class NInteger *ninteger;
    class NFloat *nfloat;
    vector<NStatement*> *statement_vector;
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
%token <nstring> TIDENTIFIER TINTEGER TFLOAT TSTRING
%token <token> KIF KELSE KWHILE KDO KINTEGER KFLOAT KSTRING KINPUT KOUTPUT KFUNCTION KEND KDEF KAS KBEGIN
%token <token> KAND KOR
%token <token> TPLUS TMINUS TMULTIPLY TDEVIDE TASSIGN
%token <token> TLESS TLESSEQUAL TGREATER TGREATEREQUAL TNOTEQUAL TEQUAL
%token <token> TLEFTBRACE TRIGHTBRACE TLEFTBRACKET TRIGHTBRACKET TSEMICOLON TCOMMA
%token <token> TERROR

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
//%type <expression> expression boolexpression
%type <statement> statement defstatement ifstatement whilestatement inputstatement outputstatement assignstatement
%type <function_statement> function
%type <statement_vector> statementblock statements
%type <function_vector> functions
%type <expression_vector> expressions
%type <identifier_vector> identifiers
%type <token> datatype relation
%type <binary_operator> expression boolexpression term factor boolterm boolfactor
%type <program> program
%type <identifier> identifier;

%start program

%%

program : functions { programBlock = new NProgram(*$1); }
	;

functions : function { $$ = new FunctionList(); $$->push_back($1); }
          | functions function { $$->push_back($2); }
          ;

function : KFUNCTION identifier TLEFTBRACKET TRIGHTBRACKET statementblock KEND KFUNCTION { $$ = new NFunctionStatement(*$2, *$5); }
         ;

statementblock : KBEGIN statements KEND { $$ = $2; }
               ;

statements : statement { $$ = new StatementList(); $$->push_back($1); }
           | statements statement { $$->push_back($2); }
           ;

statement : ifstatement | assignstatement | whilestatement | inputstatement | outputstatement | defstatement
          ;

defstatement : KDEF identifiers KAS datatype TSEMICOLON { $$ = new NDefStatement($4, *$2); }
             ;

identifiers : identifier { $$ = new IdentifierList(); $$->push_back($1); }
            | identifiers TCOMMA identifier { $1->push_back($3); }
            ;

identifier : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
           ;

datatype : KINTEGER | KFLOAT | KSTRING
         ;

inputstatement : KINPUT identifiers TSEMICOLON { $$ = new NInputStatement(*$2); }
               ;

outputstatement : KOUTPUT expressions TSEMICOLON { $$ = new NOutputStatement(*$2); }
                ;

expressions : expression { $$ = new ExpressionList(); $$->push_back($1); }
            | expressions TCOMMA expression { $1->push_back($3); }
            ;

assignstatement : identifier TASSIGN expression TSEMICOLON { $$ = new NAssignStatement(*$1, *$3); }
                ;

ifstatement : KIF boolexpression statementblock { $$ = new NIfStatement(*$2, *$3); }
            | KIF boolexpression statementblock KELSE statementblock { $$ = new NIfStatement(*$2, *$3, *$5); }
            ;

whilestatement : KWHILE boolexpression KDO statementblock { $$ = new NWhileStatement(*$2, *$4); }
               ;

expression : term { $$ = $1; }
           | expression TPLUS term { $$ = new NBinaryOperator(*$1, $2, *$3); }
           | expression TMINUS term { $$ = new NBinaryOperator(*$1, $2, *$3); }
           ;

term : factor { $$ = $1; }
     | term TMULTIPLY factor { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | term TDEVIDE factor { $$ = new NBinaryOperator(*$1, $2, *$3); }
     ;

factor : identifier { $<identifier>$ = $1; }
       | TINTEGER { $<ninteger>$ = new NInteger(atol($1->c_str())); delete $1; }
       | TFLOAT { $<nfloat>$ = new NFloat(atof($1->c_str())); delete $1; }
       | TLEFTBRACKET expression TRIGHTBRACKET { $$ = $2; }
       ;

boolexpression : boolterm { $$ = $1; }
               | boolexpression KOR boolterm { $$ = new NBinaryOperator(*$1, $2, *$3); }
               ;

boolterm : boolfactor { $$ = $1; }
         | boolterm KAND boolfactor { $$ = new NBinaryOperator(*$1, $2, *$3); }
         ;

boolfactor : expression relation expression { $$ = new NBinaryOperator(*$1, $2, *$3); }
           | TLEFTBRACKET boolexpression TRIGHTBRACKET { $$ = $2; }
           ;

relation : TLESS | TLESSEQUAL | TGREATER | TGREATEREQUAL | TEQUAL | TNOTEQUAL
         ;

%%
#include <iostream>
//using namespace std;
void yyerror(const char *s) { cout << "ERROR: " << s << endl; }
