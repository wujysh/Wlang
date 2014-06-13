/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
/* Line 2058 of yacc.c  */
#line 9 "wlang.y"

   #include "common.h"


/* Line 2058 of yacc.c  */
#line 51 "parser.hpp"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TIDENTIFIER = 258,
     TINTEGER = 259,
     TFLOAT = 260,
     TSTRING = 261,
     KIF = 262,
     KELSE = 263,
     KWHILE = 264,
     KDO = 265,
     KINTEGER = 266,
     KFLOAT = 267,
     KSTRING = 268,
     KINPUT = 269,
     KOUTPUT = 270,
     KFUNCTION = 271,
     KEND = 272,
     KDEF = 273,
     KAS = 274,
     KBEGIN = 275,
     KAND = 276,
     KOR = 277,
     TPLUS = 278,
     TMINUS = 279,
     TMULTIPLY = 280,
     TDEVIDE = 281,
     TASSIGN = 282,
     TLESS = 283,
     TLESSEQUAL = 284,
     TGREATER = 285,
     TGREATEREQUAL = 286,
     TNOTEQUAL = 287,
     TEQUAL = 288,
     TLEFTBRACE = 289,
     TRIGHTBRACE = 290,
     TLEFTBRACKET = 291,
     TRIGHTBRACKET = 292,
     TSEMICOLON = 293,
     TCOMMA = 294,
     TERROR = 295
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 14 "wlang.y"

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


/* Line 2058 of yacc.c  */
#line 125 "parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_PARSER_HPP_INCLUDED  */
