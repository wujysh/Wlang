#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Token {
    int type;
    char sVal[256];
};

#define TID             1
#define TCON            2
#define TDECI           3
#define TSTR            4
#define TIF             5
#define TELSE           6
#define TWHILE          7
#define TDO             8
#define TINTEGER        9
#define TFLOAT          10
#define TSTRING         11
#define TINPUT          12
#define TOUTPUT         13
#define TAND            14
#define TOR             15
#define TFUNCTION       16
#define TEND            17
#define TDEF            18
#define TAS             19
#define TBEGIN          20
#define TPLUS           21
#define TMINUS          22
#define TMULTIPLY       23
#define TDEVIDE         24
#define TASSIGN         25
#define TLESS           26
#define TLESSEQUAL      27
#define TGREATER        28
#define TGREATEREQUAL   29
#define TNOTEQUAL       30
#define TEQUAL          31
#define TLEFTBRACE      32
#define TRIGHTBRACE     33
#define TLEFTBRACLET    34
#define TRIGHTBRACKET   35
#define TSEMICOLON      36
#define TCOMMA          37
#define ERROR           -1

extern void parse();
extern struct Token getToken();
