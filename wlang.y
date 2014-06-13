%{
    #include "common.h"
    NProgram *programBlock; /* the top level root node of our final AST */

    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    Node *node;
    NExpression *expression;
    NStatement *statement;
    /*NDefStatement *def_statement;
    NIfStatement *if_statement;
    NWhileStatement *while_statement;
    NInputStatement *input_statement;
    NOutputStatement *output_statement;
    NAssignStatement *assign_statement;
    NFunctionStatement *function_statement;*/
    NProgram *program;
    NBinaryOperator *binary_operator;
    std::vector<NStatement*> *statement_vector;
    std::vector<NFunctionStatement*> *function_vector;
    std::vector<NExpression*> *expression_vector;
    std::vector<NIdentifier*> *identifier_vector;
    std::string *string;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TFLOAT TSTRING
%token <token> KIF KELSE KWHILE KDO KINTEGER KFLOAT KSTRING KINPUT KOUTPUT KFUNCTION KEND KDEF KAS KBEGIN
%token <token> KAND KOR
%token <token> TPLUS TMINUS TMULTIPLY TDEVIDE TASSIGN
%token <token> TLESS TLESSEQUAL TGREATER TGREATEREQUAL TNOTEQUAL TEQUAL
%token <token> TLEFTBRACE TRIGHTBRACE TLEFTBRACKET TRIGHTBRACKET TSEMICOLON TCOMMA

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <expression> expression boolexpression
%type <statement> statement defstatement ifstatement whilestatement inputstatement outputstatement assignstatement function
/*%type <def_statement>
%type <if_statement>
%type <while_statement>
%type <input_statement>
%type <output_statement>
%type <assign_statement>
%type <function_statement>*/
%type <statement_vector> statementblock statements
%type <function_vector> functions
%type <expression_vector> expressions
%type <identifier_vector> identifiers
%type <token> datatype relation
%type <binary_operator> term factor boolterm boolfactor
%type <program> program

%start program

%%

program : functions { programBlock = $1; }
	;

functions : function { $$ = new FunctionList(); $$->push_back($1); }
          | functions function { $$->push_back($2); }
          ;

function : KFUNCTION TIDENTIFIER TLEFTBRACKET TRIGHTBRACKET statementblock KEND KFUNCTION { $$ = new NFunctionStatement(NIdentifier*$2), *$5); }
         ;

statementblock : KBEGIN statements KEND { $$ = $2; }
               ;

statements : statement { $$ = new StatementList(); $$->push_back($1); }
           | statements statement { $$->push_back($2); }
           ;

statement : ifstatement | assignstatement | whilestatement | inputstatement | outputstatement | defstatement
          ;

defstatement : KDEF identifiers KAS datatype TSEMICOLON { $$ = new NDefStatement(*$4, *$2); }
             ;

identifiers : TIDENTIFIER { $$ = new IdentifierList(); $$->push_back(NIdentifier(*$1)); }
            | identifiers TCOMMA TIDENTIFIER { $1->push_back(NIdentifier(*$3)); }
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

assignstatement : TIDENTIFIER TASSIGN expression TSEMICOLON { $$ = new NAssignStatement(NIdentifier(*$1), *$3); }
                ;

ifstatement : KIF boolexpression statementblock { $$ = new NIfStatement(*$2, *$3, NULL); }
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

factor : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
       | TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
       | TFLOAT { $$ = new NDouble(atof($1->c_str())); delete $1; }
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
