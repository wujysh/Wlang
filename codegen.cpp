#include "codegen.h"
#include "parser.hpp"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NProgram& root) {
    std::cout << "Generating code...\n";

    /* Create the top level interpreter function to call as entry */
    //vector<const Type*> argTypes;
    //FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root.codeGen(*this); /* emit bytecode for the toplevel block */
    ReturnInst::Create(getGlobalContext(), bblock);
    popBlock();

    /* Print the bytecode in a human-readable format
       to see if our program compiled properly
     */
    std::cout << "Code is generated.\n";
    //PassManager pm;
    //pm.add(createPrintModulePass(&outs()));
    //pm.run(*module);
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
    std::cout << "Running code...\n";
//	ExistingModuleProvider *mp = new ExistingModuleProvider(module);
//	ExecutionEngine *ee = ExecutionEngine::create(mp, false);
//	vector<GenericValue> noargs;
//	GenericValue v = ee->runFunction(mainFunction, noargs);
    std::cout << "Code was run.\n";
//	return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(int type) {
    if (type == KINTEGER) {
        return Type::getInt64Ty(getGlobalContext());
    } else if (type == KFLOAT) {
        return Type::getDoubleTy(getGlobalContext());
    } else if (type == KSTRING) {
        //return Type::getStringTy(getGlobalContext());
    }
    return Type::getVoidTy(getGlobalContext());
}

/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context) {
    std::cout << "Creating integer: " << value << std::endl;
    return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

Value* NFloat::codeGen(CodeGenContext& context) {
    std::cout << "Creating float: " << value << std::endl;
    return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
    std::cout << "Creating identifier reference: " << name << std::endl;
    if(context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << std::endl;
        return NULL;
    }
    return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

//Value* NMethodCall::codeGen(CodeGenContext& context)
//{
//	Function *function = context.module->getFunction(id.name.c_str());
//	if (function == NULL) {
//		std::cerr << "no such function " << id.name << std::endl;
//	}
//	std::vector<Value*> args;
//	ExpressionList::const_iterator it;
//	for (it = arguments.begin(); it != arguments.end(); it++) {
//		args.push_back((**it).codeGen(context));
//	}
//	CallInst *call = CallInst::Create(function, args.begin(), args.end(), "", context.currentBlock());
//	std::cout << "Creating method call: " << id.name << std::endl;
//	return call;
//}

Value* NBinaryOperator::codeGen(CodeGenContext& context) {
    std::cout << "Creating binary operation " << op << std::endl;
    Instruction::BinaryOps instr;
    switch(op) {
    case TPLUS:
        instr = Instruction::Add;
        goto math;
    case TMINUS:
        instr = Instruction::Sub;
        goto math;
    case TMULTIPLY:
        instr = Instruction::Mul;
        goto math;
    case TDEVIDE:
        instr = Instruction::SDiv;
        goto math;

        /* TODO comparison */
    }

    return NULL;
math:
    return BinaryOperator::Create(instr, lhs.codeGen(context),
                                  rhs.codeGen(context), "", context.currentBlock());
}

Value* NAssignStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating assignment for " << lhs.name << std::endl;
    if(context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "undeclared variable " << lhs.name << std::endl;
        return NULL;
    }
    return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

Value* NIfStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating if statement " << std::endl;
}

Value* NWhileStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating while statement " << std::endl;
}

Value* NInputStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating input statement " << std::endl;
}

Value* NOutputStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating output statement " << std::endl;
}

Value* NFunctionStatement::codeGen(CodeGenContext& context) {
    StatementList::const_iterator it;
    Value *last = NULL;
    for(it = block.begin(); it != block.end(); it++) {
        std::cout << "Generating code for " << typeid(**it).name() << std::endl;
        last = (**it).codeGen(context);
    }
    std::cout << "Creating block" << std::endl;
    return last;
}

//Value* NExpressionStatement::codeGen(CodeGenContext& context) {
//    std::cout << "Generating code for " << typeid(expression).name() << std::endl;
//    return expression.codeGen(context);
//}

Value* NDefStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating variable declaration " << type << " ";
    IdentifierList::iterator it;
    AllocaInst *alloc;
    for (it = identifiers.begin(); it != identifiers.end(); it++) {
        std::cout << (*it)->name << " ";
        alloc = new AllocaInst(typeOf(type), (*it)->name.c_str(), context.currentBlock());
        context.locals()[(*it)->name] = alloc;
    }
    std::cout << std::endl;
    //std::cout << "Creating variable declaration " << type << " " << id.name << std::endl;
    //AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
    //context.locals()[id.name] = alloc;
    //if (assignmentExpr != NULL) {
    //    NAssignment assn(id, *assignmentExpr);
    //    assn.codeGen(context);
    //}
    return alloc;
}

//Value* NFunctionDeclaration::codeGen(CodeGenContext& context) {
//    vector<const Type*> argTypes;
//    VariableList::const_iterator it;
//    for(it = arguments.begin(); it != arguments.end(); it++) {
//        argTypes.push_back(typeOf((**it).type));
//    }
//    FunctionType *ftype = FunctionType::get(typeOf(type), argTypes, false);
//    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
//    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
//
//    context.pushBlock(bblock);
//
//    for(it = arguments.begin(); it != arguments.end(); it++) {
//        (**it).codeGen(context);
//    }
//
//    block.codeGen(context);
//    ReturnInst::Create(getGlobalContext(), bblock);
//
//    context.popBlock();
//    std::cout << "Creating function: " << id.name << std::endl;
//    return function;
//}

Value* NProgram::codeGen(CodeGenContext& context) {
    FunctionList::iterator it;
    for (it = functions.begin(); it != functions.end(); it++) {
        //vector<const Type*> argTypes;
        FunctionType *ftype = FunctionType::get(typeOf(0), false);
        Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, (*it)->id.name.c_str(), context.module);
        BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

        context.pushBlock(bblock);

        (**it).codeGen(context);

        ReturnInst::Create(getGlobalContext(), bblock);

        context.popBlock();
        std::cout << "Creating function: " << (*it)->id.name << std::endl;
        return function;
    }
}
