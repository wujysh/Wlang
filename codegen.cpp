#include "codegen.h"
#include "parser.hpp"

using namespace std;
using namespace llvm;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NProgram& root) {
    std::cout << "Generating code...\n";

    root.codeGen(*this); /* emit bytecode for the toplevel block */

    /* Print the bytecode in a human-readable format
       to see if our program compiled properly
     */
    std::cout << "Code is generated.\n";
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
    std::cout << "PassManager finished." << std::endl;
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
static Type *typeOf(int type)
{
    if (type == INTEGER) {
        return Type::getInt64Ty(getGlobalContext());
    } else if (type == FLOAT) {
        return Type::getDoubleTy(getGlobalContext());
    } else if (type == STRING) {
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

    bool found = false;
    std::map<std::string, Value*>& locals = context.locals();
    for (auto rit = context.blocks.rbegin(); rit != context.blocks.rend(); ++rit) { 
        locals = context.locals();
        if (locals.find(name) != locals.end()) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cerr << "undeclared variable " << name << std::endl;
        return nullptr;
    }
    return new LoadInst(locals[name], "", false, context.currentBlock());
}

bool NBinaryOperator::autoUpgradeType(CodeGenContext& context,
                                      Value *lhs, Value *rhs)
{
    bool isConverted = false;
    // conversion if it exists a float type
    if (lhs->getType()->isDoubleTy() || rhs->getType()->isDoubleTy()) {
        if (!lhs->getType()->isDoubleTy()) {
            lhs = new SIToFPInst(lhs, Type::getFloatTy(context.module->getContext()),
                                 "conv", context.currentBlock());
            isConverted = true;
        }
        if (!rhs->getType()->isDoubleTy()) {
            rhs = new SIToFPInst(rhs, Type::getFloatTy(context.module->getContext()),
                                 "conv", context.currentBlock());
            isConverted = true;
        }
    }
    return isConverted;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context) {
    std::cout << "Creating binary operation " << op << std::endl;
    Instruction::BinaryOps instr;
    Instruction::OtherOps otherInstr;
    CmpInst::Predicate pred;
    Value *L = lhs.codeGen(context);
    Value *R = rhs.codeGen(context);
    switch (op) {
    case TPLUS:
        instr = Instruction::Add;
        if (autoUpgradeType(context, L, R))
            instr = Instruction::FAdd;
        goto math;
    case TMINUS:
        instr = Instruction::Sub;
        if (autoUpgradeType(context, L, R))
            instr = Instruction::FSub;
        goto math;
    case TMULTIPLY:
        instr = Instruction::Mul;
        if (autoUpgradeType(context, L, R))
            instr = Instruction::FMul;
        goto math;
    case TDIVIDE:
        instr = Instruction::SDiv;
        if (autoUpgradeType(context, L, R))
            instr = Instruction::FDiv;
        goto math;
    case TLESS:
        otherInstr = Instruction::ICmp;
        pred = CmpInst::ICMP_SLT;
        if (autoUpgradeType(context, L, R)) {
            pred = CmpInst::FCMP_ULT;
            otherInstr = Instruction::FCmp;
        }
        goto relation;
    case TGREATER:
        otherInstr = Instruction::ICmp;
        pred = CmpInst::ICMP_SGT;
        if (autoUpgradeType(context, L, R)) {
            pred = CmpInst::FCMP_UGT;
            otherInstr = Instruction::FCmp;
        }
        goto relation;
    case TLESSEQUAL:
        otherInstr = Instruction::ICmp;
        pred = CmpInst::ICMP_SLE;
        if (autoUpgradeType(context, L, R)) {
            pred = CmpInst::FCMP_ULE;
            otherInstr = Instruction::FCmp;
        }
        goto relation;
    case TGREATEREQUAL:
        otherInstr = Instruction::ICmp;
        pred = CmpInst::ICMP_UGE;
        if (autoUpgradeType(context, L, R)) {
            pred = CmpInst::FCMP_UGE;
            otherInstr = Instruction::FCmp;
        }
        goto relation;
    case TEQUAL:
        otherInstr = Instruction::ICmp;
        pred = CmpInst::ICMP_EQ;
        if (autoUpgradeType(context, L, R)) {
            pred = CmpInst::FCMP_UEQ;
            otherInstr = Instruction::FCmp;
        }
        goto relation;
    case TNOTEQUAL:
        otherInstr = Instruction::ICmp;
        pred = CmpInst::ICMP_NE;
        if (autoUpgradeType(context, L, R)) {
            pred = CmpInst::FCMP_UNE;
            otherInstr = Instruction::FCmp;
        }
        goto relation;
    case AND:
        instr = Instruction::And;
        goto math;
    case OR:
        instr = Instruction::Or;
        goto math;
    default:
        std::cerr << "Invalid binary operator" << std::endl;
    }
    return nullptr;
math:
    return BinaryOperator::Create(instr, L, R, "math", context.currentBlock());
relation:
    CmpInst *compareResult = CmpInst::Create(otherInstr, pred, L, R, "cmptmp", context.currentBlock());
    // Convert bool 0/1 to double 0.0 or 1.0
    return CastInst::Create(Instruction::UIToFP, compareResult,
                            Type::getDoubleTy(getGlobalContext()), "booltmp");
}

Value* NAssignStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating assignment for " << identifier.name << std::endl;

    bool found = false;
    std::map<std::string, Value*>& locals = context.locals();
    for (auto rit = context.blocks.rbegin(); rit != context.blocks.rend(); ++rit) {
        locals = context.locals();
        if (locals.find(identifier.name) != locals.end()) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cerr << "undeclared variable " << identifier.name << std::endl;
        return nullptr;
    }

    return new StoreInst(value.codeGen(context), locals[identifier.name], false, context.currentBlock());
}

Value* NArgument::codeGen(CodeGenContext& context) {
    std::cout << "Creating argument " << identifier.name << std::endl;
    return nullptr;
}

Value* NIfStatement::codeGen(CodeGenContext& context)
{
    std::cout << "Creating if statement " << std::endl;

    Value *condValue = condition.codeGen(context);
    if (condValue == nullptr) return nullptr;

    std::cout << condValue->getType()->getTypeID() << std::endl;
    condValue = new FCmpInst(*context.currentBlock(), CmpInst::FCMP_ONE,
                             condValue, ConstantFP::get(getGlobalContext(), APFloat(0.0)));
    Function *function = context.currentBlock()->getParent();

    BasicBlock *thenBlock = BasicBlock::Create(getGlobalContext(), "if.then", function);
    BasicBlock *elseBlock = BasicBlock::Create(getGlobalContext(), "if.else");
    BasicBlock *mergeBlock = BasicBlock::Create(getGlobalContext(), "if.cont");

    BranchInst::Create(thenBlock, elseBlock, condValue, context.currentBlock());


    // create then block
    context.pushBlock(thenBlock);

    Value *thenValue = conditionCodeGen(context, thenblock);
    if (thenValue == nullptr) return nullptr;
    BranchInst::Create(mergeBlock, context.currentBlock());

    context.popBlock();

    // create else block
    function->getBasicBlockList().push_back(elseBlock);
    context.pushBlock(elseBlock);

    Value *elseValue = conditionCodeGen(context, elseblock);
    if (elseValue == nullptr) return nullptr;
    BranchInst::Create(mergeBlock, context.currentBlock());

    context.popBlock();

    // create PHI node
    function->getBasicBlockList().push_back(mergeBlock);
    context.pushBlock(mergeBlock);

    PHINode *PN = PHINode::Create(Type::getVoidTy(getGlobalContext()), 2, "if.tmp", mergeBlock);
    PN->addIncoming(thenValue, thenBlock);
    PN->addIncoming(elseValue, elseBlock);

    context.popBlock();

    return PN;
}

Value* NIfStatement::conditionCodeGen(CodeGenContext& context, StatementList& block)
{
    std::cout << "Generate conditional block" << std::endl;
    StatementList::const_iterator it;
    Value *last = nullptr;
    for (it = block.begin(); it != block.end(); it++) {
        std::cout << "Generating code for " << typeid(**it).name() << ' ' << std::endl;
        last = (**it).codeGen(context);
    }
    return last;
}

Value* NWhileStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating while statement " << std::endl;
    return nullptr;
}

Value* NInputStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating input statement " << std::endl;
    return nullptr;
}

Value* NOutputStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating output statement " << std::endl;
    return nullptr;
}

Value* NReturnStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating return statement " << std::endl;
    return NULL;
}

Value* NMethodCall::codeGen(CodeGenContext& context) {
    std::cout << "Creating method call " << std::endl;
    return NULL;
}

Value* NExprStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating expression statement " << std::endl;
    return NULL;
}

Value* NFunction::codeGen(CodeGenContext& context)
{
    vector<Type*> argTypes;
    for (ArgumentList::const_iterator it = arguments.begin();
         it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    /* Create the top level interpreter function to call as entry */
    FunctionType *ftype = FunctionType::get(typeOf(returnType), false);
    auto name = id.name;
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, name.c_str(), context.module);

    if (name == "main")
        context.mainFunction = function;

    // If F conflicted, there was already something named 'Name'.  If it has a
    // body, don't allow redefinition or reextern.
    if (function->getName() != name) {
        // Delete the one we just made and get the existing one.
        function->eraseFromParent();
        function = context.module->getFunction(name);
    }
    /* Push a new variable/block context */
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
    context.pushBlock(bblock);

    Value *last = nullptr;
    for (StatementList::const_iterator it = block.begin();
         it != block.end(); it++) {
        std::cout << "Generating code for " << typeid(**it).name() << ' ' << std::endl;
        last = (**it).codeGen(context);
    }

    ReturnInst::Create(getGlobalContext(), bblock);

    context.popBlock();

    return last;
}

Value* NDefStatement::codeGen(CodeGenContext& context)
{
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

Value* NProgram::codeGen(CodeGenContext& context)
{
    std::cout << "Generating program." << std::endl;
    FunctionList::iterator it;
    Value *last = nullptr;
    for (it = functions.begin(); it != functions.end(); it++) {
        std::cout << "Creating function: " << (*it)->id.name << std::endl;
        last = (**it).codeGen(context);
    }
    return last;
}
