#ifndef AST_H_
#define AST_H_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include "llvm/IR/Value.h"
using namespace std;

class CodeGenContext;
class Node;
class NStatement;
class NExpression;
class NFunctionStatement;
class NIdentifier;
class NInteger;
class NFloat;
class NString;
class NProgram;
class NBinaryOperator;
class NAssignStatement;
class NIfStatement;
class NWhileStatement;
class NInputStatement;
class NOutputStatement;

typedef vector<NStatement*> StatementList;
typedef vector<NFunctionStatement*> FunctionList;
typedef vector<NExpression*> ExpressionList;
typedef vector<NIdentifier*> IdentifierList;

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

class NDefStatement : public NStatement {
public:
    int type;
    IdentifierList identifiers;
    NDefStatement(int type, IdentifierList& identifiers) :
        type(type), identifiers(identifiers) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIfStatement : public NStatement {
public:
    NExpression& condition;
    StatementList thenblock;
    StatementList elseblock;
    NIfStatement(NExpression& condition, StatementList& thenblock) :
        condition(condition), thenblock(thenblock) {}
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
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionStatement : public NStatement {
public:
    const NIdentifier& id;
    StatementList block;
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

#endif // AST_H_
