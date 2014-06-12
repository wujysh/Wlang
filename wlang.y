%{
    #include "node.h"
    NBlock *programBlock; /* the top level root node of our final AST */

    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NVariableDeclaration *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
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
%token <token> TLEFTBRACE TRIGHTBRACE TLEFTBRACLET TRIGHTBRACKET TSEMICOLON TCOMMA

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> numeric expr 
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl
%type <token> datatype relation

%start program

%%

program : functions { programBlock = $1; }
	;

functions : function { $$ = new FunctionList(); $$->push_back($1); }
          | functions function { $$->push_back($2); }
          ;

function : KFUNCTION TIDENTIFIER TLEFTBRACKET TRIGHTBRACKET statementblock KEND KFUNCTION { $$ = $5 }
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

identifiers : TIDENTIFIER { $$ = new IdentifierList(); $$->push_back($1); }
            | identifiers TCOMMA TIDENTIFIER { $1->push_back($3); }
            ;

datatype : KINTEGER | KFLOAT | KSTRING
         ;

inputstatement : KINPUT identifiers TSEMICOLON { $$ = new NInputStatement(); }
               ;

outputstatement : KOUTPUT expressions TSEMICOLON { $$ = new NOutputStatement(); }
                ;

expressions : expression { $$ = new ExpressionList(); $$->push_back($1); }
            | expressions TCOMMA expression { $1->push_back($3); }
            ;

assignstatement : TIDENTIFIER TASSIGN expression TSEMICOLON {}
                ;

ifstatement : KIF boolexpression statementblock { $$ = new NIfStatement(); }
            | KIF boolexpression statementblock KELSE statementblock { $$ = new NIfStatement(); }
            ;

whilestatement : KWHILE boolexpression KDO statementblock { $$ = new NWhileStatement(); }
               ;

expression : term {}
           | expression TPLUS term { $$ = new NBinaryOperator(*$1, $2, *$3); }
           | expression TMINUS term { $$ = new NBinaryOperator(*$1, $2, *$3); }
           ;

term : factor {}
     | term TMULTIPLY factor { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | term TDEVIDE factor { $$ = new NBinaryOperator(*$1, $2, *$3); }
     ;

factor : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
       | TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
       | TFLOAT { $$ = new NDouble(atof($1->c_str())); delete $1; }
       | TLEFTBRACKET expression TRIGHTBRACKET {}
       ;

boolexpression : boolterm {}
               | boolexpression KOR boolterm {}
               ;

boolterm : boolfactor {}
         | boolterm KAND boolfactor {}
         ;

boolfactor : expression relation expression { $$ = new NBinaryOperator(*$1, $2, *$3); }
           | TLEFTBRACKET boolexpression TRIGHTBRACKET {}
           ;

relation : TLESS | TLESSEQUAL | TGREATER | TGREATEREQUAL | TEQUAL | TNOTEQUAL
         ;


//============================================================
program : stmts { programBlock = $1; }
        ;
        
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : var_decl | func_decl
     | expr { $$ = new NExpressionStatement(*$1); }
     ;

block : KBEGIN stmts KEND { $$ = $2; }
      | KBEGIN KEND { $$ = new NBlock(); }
      ;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
	: KDEF ident KAS ident { $$ = new NVariableDeclaration(*$4, *$2); }
         | ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
         ;
        
func_decl : ident ident TLPAREN func_decl_args TRPAREN block 
            { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
          ;
    
func_decl_args : /*blank*/  { $$ = new VariableList(); }
          | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
          | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
          ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
      ;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
        | TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
	|
        ;
    
expr : ident TEQUAL expr { $$ = new NAssignment(*$<ident>1, *$3); }
     //| ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
     | ident { $<ident>$ = $1; }
     | numeric
     | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
     ;
    
call_args : /*blank*/  { $$ = new ExpressionList(); }
          | expr { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args TCOMMA expr  { $1->push_back($3); }
          ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE 
           | TPLUS | TMINUS | TMUL | TDIV
           ;

%%
