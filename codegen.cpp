#include "codegen.h"
#include "parser.hpp"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NProgram& root) {
    std::cout << "Generating code...\n";

    /* Create the top level interpreter function to call as entry */
    //vector<const Type*> argTypes;
    //FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
    //FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
    //mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    //BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

    /* Push a new variable/block context */
    //pushBlock(bblock);
    root.codeGen(*this); /* emit bytecode for the toplevel block */
    //ReturnInst::Create(getGlobalContext(), bblock);
    //popBlock();

    /* Print the bytecode in a human-readable format
       to see if our program compiled properly
     */
    std::cout << "Code is generated.\n";
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
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
        //return Type::getLabelTy(getGlobalContext());
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

Value* NString::codeGen(CodeGenContext& context) {
    std::cout << "Creating string: " << value << std::endl;
    //return ConstantArray::get(Type::getLabelTy(getGlobalContext()), value.c_str());
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
    std::cout << "Creating identifier reference: " << name << std::endl;
    if(context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << std::endl;
        return NULL;
    }
    return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

Value* NBinaryOperator::codeGen(CodeGenContext& context) {
    std::cout << "Creating binary operation " << op << std::endl;
    Instruction::BinaryOps instr;
    switch (op) {
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
    case TLESS:
        //instr = Instruction::Less
        goto math;
    case TGREATER:

        goto math;
    case TLESSEQUAL:

        goto math;
    case TGREATEREQUAL:

        goto math;
    case TEQUAL:

        goto math;
    case TNOTEQUAL:

        goto math;
    case KAND:
        instr = Instruction::And;
        goto math;
    case KOR:
        instr = Instruction::Or;
        goto math;
        /* TODO comparison */
    }

    return NULL;
math:
    return BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
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
    return NULL;
}

Value* NWhileStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating while statement " << std::endl;
    return NULL;
}

Value* NInputStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating input statement " << std::endl;
    return NULL;
}

Value* NOutputStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating output statement " << std::endl;
    return NULL;
}

Value* NFunctionStatement::codeGen(CodeGenContext& context) {
    //vector<const Type*> argTypes;
    FunctionType *ftype = FunctionType::get(typeOf(0), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    context.pushBlock(bblock);

    StatementList::const_iterator it;
    Value *last = NULL;
    for(it = block.begin(); it != block.end(); it++) {
        std::cout << "Generating code for " << typeid(**it).name() << std::endl;
        last = (**it).codeGen(context);
    }

    ReturnInst::Create(getGlobalContext(), bblock);

    context.popBlock();

    return last;
}

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
    return alloc;
}

Value* NProgram::codeGen(CodeGenContext& context) {
    FunctionList::iterator it;
    Value *last = NULL;
    for (it = functions.begin(); it != functions.end(); it++) {
        std::cout << "Creating function: " << (*it)->id.name << std::endl;
        last = (**it).codeGen(context);
    }
    return last;
}
