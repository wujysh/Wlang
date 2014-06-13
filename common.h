#include <iostream>
#include <vector>
#include <llvm/Value.h>

#define TIDENTIFIER     1
#define TINTEGER        2
#define TFLOAT          3
#define TSTRING         4
#define KIF             5
#define KELSE           6
#define KWHILE          7
#define KDO             8
#define KINTEGER        9
#define KFLOAT          10
#define KSTRING         11
#define KINPUT          12
#define KOUTPUT         13
#define KAND            14
#define KOR             15
#define KFUNCTION       16
#define KEND            17
#define KDEF            18
#define KAS             19
#define KBEGIN          20
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
#define TERROR          -1

class CodeGenContext;
class NStatement;
class NExpression;
class NFunctionStatement;
class NIdentifier;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NFunctionStatement*> FunctionList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NIdentifier*> IdentifierList;

class Node {
public:
    virtual ~Node() {}
    virtual llvm::Value* codeGen(CodeGenContext& context) { }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFloat : public NExpression {
public:
    double value;
    NFloat(double value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NString : public NExpression {
public:
    std::string value;
    NString(const std::string& value) : value(value) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

//class NMethodCall : public NExpression {
//public:
//    const NIdentifier& id;
//    ExpressionList arguments;
//    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
//        id(id), arguments(arguments) { }
//    NMethodCall(const NIdentifier& id) : id(id) { }
//    virtual llvm::Value* codeGen(CodeGenContext& context);
//};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignStatement : public NStatement {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignStatement(NIdentifier& lhs, NExpression& rhs) :
        lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

//class NBlock : public NExpression {
//public:
//    StatementList statements;
//    NBlock() {}
//    virtual llvm::Value* codeGen(CodeGenContext& context);
//};

//class NExpressionStatement : public NStatement {
//public:
//    NExpression& expression;
//    NExpressionStatement(NExpression& expression) :
//        expression(expression) { }
//    virtual llvm::Value* codeGen(CodeGenContext& context);
//};

class NDefStatement : public NStatement {
public:
    const NIdentifier& type;
    NIdentifiers& identifiers;
    //NExpression *assignmentExpr;
    NDefStatement(const NIdentifier& type, NIdentifiers& identifiers) :
        type(type), identifiers(identifiers) { }
    //NDefStatement(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
    //    type(type), id(id), assignmentExpr(assignmentExpr) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIfStatement : public NStatement {
public:
    NExpression& condition;
    StatementList thenblock;
    StatementList elseblock;
    NIfStatement(NExpression& condition, StatementList& thenblock, StatementList& elseblock) :
        condition(condition), thenblock(thenblock), elseblock(elseblock) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NWhileStatement : public NStatement {
public:
    NExpression& condition;
    StatementList block;
    NWhileStatement(NExpression& condition, StatementList& block) :
        condition(condition), block(block) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NInputStatement : public NStatement {
public:
    IdentifierList identifiers;
    NInputStatement(IdentifierList& identifiers) : identifiers(identifiers) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NOutputStatement : public NStatement {
public:
    ExpressionList expressions;
    NOutputStatement(ExpressionList& expressions) : expressions(expressions) {}
};

class NFunctionStatement : public NStatement {
public:
    //const NIdentifier& type;
    const NIdentifier& id;
    //VariableList arguments;
    StatementList block;
    //NFunctionStatement(const NIdentifier& type, const NIdentifier& id,
    //                     const VariableList& arguments, NBlock& block) :
    //    type(type), id(id), arguments(arguments), block(block) { }
    NFunctionStatement(const NIdentifier& id, StatementList& block) :
        id(id), block(block) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NProgram : public NStatement {
public:
    FunctionList functions;
    NProgram(FunctionList& functions) : functions(functions) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};
