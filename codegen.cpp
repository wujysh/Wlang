#include "codegen.h"
#include "parser.hpp"

using namespace std;
using namespace llvm;

extern char filename[500];
extern vector<string> buf;
extern int yynerrs;

void CodeGenContext::runLLVMOptimizations()
{
/*  
    // Set up the function-level optimizations we want
    llvm::legacy::FunctionPassManager passManager(module);
    
    //llvm::PassManager passManager;
    passManager.add(llvm::createPromoteMemoryToRegisterPass());
    passManager.add(llvm::createReassociatePass());
    passManager.add(llvm::createGVNPass());
    passManager.add(llvm::createAggressiveDCEPass());
    passManager.add(llvm::createVerifierPass());
    
    llvm::Module::iterator function, lastFunction;

    // run them across all functions
    passManager.doInitialization();
    for (function = module->begin(); function != module->end(); ++function) {
        passManager.run(*function);
    }
    passManager.doFinalization();
*/
    std::cout << "======================" << std::endl;
    std::cout << "Optimizations started." << std::endl;

    llvm::legacy::PassManager passManager;

    //passManager.add(llvm::createStripDeadPrototypesPass());
    //passManager.add(llvm::createSimplifyLibCallsPass());
    //passManager.add(llvm::createArgumentPromotionPass());
    //passManager.add(llvm::createDeadArgEliminationPass());

    passManager.add(llvm::createBasicAliasAnalysisPass());
//  passManager.add(llvm::createGVNPass());
    passManager.add(llvm::createLICMPass());
    passManager.add(llvm::createDeadInstEliminationPass());
    passManager.add(llvm::createLCSSAPass());
    passManager.add(llvm::createLoopIdiomPass());
    passManager.add(llvm::createLoopInstSimplifyPass());
    passManager.add(llvm::createLoopSimplifyPass());
    passManager.add(llvm::createIndVarSimplifyPass());
    passManager.add(llvm::createLoopStrengthReducePass());
    passManager.add(llvm::createLoopUnrollPass());

    //passManager.add(llvm::createFunctionAttrsPass());
    //passManager.add(llvm::createFunctionInliningPass());
    //passManager.add(llvm::createFunctionInliningPass());
    passManager.add(llvm::createDeadStoreEliminationPass());
    passManager.add(llvm::createDeadCodeEliminationPass());

    passManager.add(llvm::createReassociatePass());
    //passManager.add(llvm::createConstantMergePass());
    passManager.add(llvm::createConstantPropagationPass());
    passManager.add(llvm::createSROAPass());
    passManager.add(llvm::createInstructionSimplifierPass());
    //passManager.add(llvm::createDeadArgEliminationPass());
    //passManager.add(llvm::createBBVectorizePass());

    //passManager.add(llvm::createGlobalOptimizerPass());
    passManager.add(llvm::createGlobalsModRefPass());

    //passManager.add(llvm::createPartialInliningPass());
//  passManager.add(llvm::createPartialSpecializationPass());

    passManager.run(*module);

    std::cout << "Optimizations finished." << std::endl;
}

/* Compile the AST into a module */
void CodeGenContext::generateCode(NProgram& root) {
    std::cout << "Generating code...\n";

    root.codeGen(*this); /* emit bytecode for the toplevel block */

    std::cout << "Code is generated.\n";
    
    /* Print the bytecode in a human-readable format
       to see if our program compiled properly
     */
    module->dump();

    runLLVMOptimizations();

    // Print out all of the generated code.
    module->dump();
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
    std::cout << "Running code...\n";
	// ExecutionEngine *ee = ExecutionEngine::create(module, nullptr);
 //    assert(ee != nullptr);
	// vector<GenericValue> noargs;
 //    assert(mainFunction != nullptr);
 //    GenericValue v = ee->runFunction(mainFunction, noargs);
    
    GenericValue v = GenericValue();
    std::cout << "Code was run.\n";
	return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(int type)
{
    if (type == INTEGER) {
        return Type::getInt64Ty(getGlobalContext());
    } else if (type == FLOAT) {
        return Type::getDoubleTy(getGlobalContext());
    } else if (type == STRING) {
        return Type::getInt8PtrTy(getGlobalContext());
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
    auto str = ConstantDataArray::getString(getGlobalContext(), StringRef(value));
    return new GlobalVariable(context.getModule(), str->getType(),
                              true, GlobalValue::LinkerPrivateLinkage,
                              str);
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
    std::cout << "Creating identifier reference: " << name << std::endl;

    bool found = false;
    std::map<std::string, Value*> locals;
    for (auto rit = context.blocks.rbegin(); rit != context.blocks.rend(); ++rit) {
        locals = (**rit).locals;
        if (locals.find(name) != locals.end()) {
            found = true;
            break;
        }
    }
    if (!found) {
        llvmerror("semantic error: undeclared variable", line, column, length);
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
    if (L == nullptr || R == nullptr) return nullptr;

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
        llvmerror("semantic error: invalid binary operator", line, column, length);
    }
    return nullptr;
math:
    return BinaryOperator::Create(instr, L, R, "math", context.currentBlock());
relation:
    CmpInst *compareResult = CmpInst::Create(otherInstr, pred, L, R, "cmptmp", context.currentBlock());
    // Convert bool 0/1 to double 0.0 or 1.0
    return CastInst::Create(Instruction::UIToFP, compareResult,
                            Type::getDoubleTy(getGlobalContext()), "booltmp", context.currentBlock());
}

Value* NAssignStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating assignment for " << identifier.name << std::endl;

    bool found = false;
    std::map<std::string, Value*> locals;
    for (auto rit = context.blocks.rbegin(); rit != context.blocks.rend(); ++rit) {
        locals = (**rit).locals;
        if (locals.find(identifier.name) != locals.end()) {
            found = true;
            break;
        }
    }
    if (!found) {
        llvmerror("semantic error: undeclared variable", line, column, length);
        return nullptr;
    }

    auto v = value.codeGen(context);
    if (v->getType()->getTypeID() == 14) { // pointer, just for string
        std::cout << "GetElementPtrInst" << std::endl;
        auto gep = GetElementPtrInst::Create(locals[identifier.name], v, "",context.currentBlock());
        return gep;
    }
    return new StoreInst(v, locals[identifier.name], false, context.currentBlock());
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

    //std::cout << condValue->getType()->getTypeID() << std::endl;
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
    BranchInst::Create(mergeBlock, context.currentBlock());

    context.popBlock();

    // create else block
    function->getBasicBlockList().push_back(elseBlock);
    context.pushBlock(elseBlock);

    Value *elseValue = conditionCodeGen(context, elseblock);
    BranchInst::Create(mergeBlock, context.currentBlock());

    context.popBlock();

    // create PHI node
    function->getBasicBlockList().push_back(mergeBlock);
    context.pushBlock(mergeBlock);

    PHINode *PN = PHINode::Create(Type::getDoubleTy(getGlobalContext()), 2, "if.tmp", mergeBlock);
    std::cout << thenValue->getType()->getTypeID() << std::endl;
    PN->setIncomingBlock(0, thenBlock);
    PN->setIncomingBlock(1, elseBlock);
    ReturnInst::Create(getGlobalContext(), PN, mergeBlock);

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

    Function *function = context.currentBlock()->getParent();

    BasicBlock *condBlock = BasicBlock::Create(getGlobalContext(), "while.cond", function);
    BasicBlock *loopBlock = BasicBlock::Create(getGlobalContext(), "while.loop");
    BasicBlock *afterBlock = BasicBlock::Create(getGlobalContext(), "while.after");
    
    // branch to condition block
    BranchInst::Create(condBlock, context.currentBlock());

    // create cont block
    context.pushBlock(condBlock);

    Value *condValue = condition.codeGen(context);
    if (condValue == nullptr) {
        context.popBlock();
        function->getBasicBlockList().pop_back();
        return nullptr;
    }
    condValue = new FCmpInst(*context.currentBlock(), CmpInst::FCMP_ONE,
                             condValue, ConstantFP::get(getGlobalContext(), APFloat(0.0)));
    BranchInst::Create(loopBlock, afterBlock, condValue, context.currentBlock());

    context.popBlock();

    // create loop block
    function->getBasicBlockList().push_back(loopBlock);
    context.pushBlock(loopBlock);

    Value *loopValue = conditionCodeGen(context, block);
    if (loopValue == nullptr) {
        context.popBlock();
        function->getBasicBlockList().pop_back();
        function->getBasicBlockList().pop_back();
        return nullptr;
    }
    BranchInst::Create(condBlock, context.currentBlock());

    context.popBlock();

    // create PHI node
    function->getBasicBlockList().push_back(afterBlock);
    context.pushBlock(afterBlock);

    PHINode *PN = PHINode::Create(Type::getVoidTy(getGlobalContext()), 2, "while.tmp", afterBlock);
    PN->addIncoming(loopValue, loopBlock);
    PN->addIncoming(condValue, condBlock);
    ReturnInst::Create(getGlobalContext(), PN, afterBlock);

    context.popBlock();

    return PN;
}

Value* NWhileStatement::conditionCodeGen(CodeGenContext& context, StatementList& block)
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
    if (value == nullptr) {
        return ReturnInst::Create(getGlobalContext(), context.currentBlock());
    } else {
        return ReturnInst::Create(getGlobalContext(), value->codeGen(context), context.currentBlock());
    }
}

Value* NMethodCall::codeGen(CodeGenContext& context) {
    std::cout << "Creating method call " << id.name << std::endl;
    Function *function = context.module->getFunction(id.name.c_str());
    if (function == nullptr) {
        llvmerror("semantic error: no such function", line, column, length);
        return nullptr;
    }
    std::vector<Value*> args;
    for (ExpressionList::const_iterator it = arguments.begin();
         it != arguments.end(); it++) {
        args.push_back((**it).codeGen(context));
    }
    return CallInst::Create(function, args, "", context.currentBlock());
}

Value* NExprStatement::codeGen(CodeGenContext& context) {
    std::cout << "Creating expression statement " << std::endl;
    Value *last = nullptr;
    for (ExpressionList::const_iterator it = expressions.begin();
         it != expressions.end(); it++) {
        last = (**it).codeGen(context);
    }
    return last;
}

Value* NFunction::codeGen(CodeGenContext& context)
{
    vector<Type*> argTypes;
    for (ArgumentList::const_iterator it = arguments.begin();
         it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    /* Create the top level interpreter function to call as entry */
    FunctionType *ftype = FunctionType::get(typeOf(returnType), argTypes, false);
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
        if (context.locals().find((*it)->name) != context.locals().end()) {
            llvmerror("semantic error: redefinition", line, column, length);
            return nullptr;
        }
        if (type != STRING) {
            alloc = new AllocaInst(typeOf(type), (*it)->name.c_str(), context.currentBlock());

        } else {
            alloc = new AllocaInst(typeOf(type), nullptr, (*it)->name.c_str(), context.currentBlock());
        }
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

void llvmerror(char const *s, int& line, int& column, int& length) {
    printf("%s:%d:%d: %s\n%s\n", filename, line, column, s, buf[line].c_str());
    printf("%*s", column-1, "");
    for (int i = 0; i < length; i++) {
        printf("%c", '^');
    }
    printf("\n");
    yynerrs++;
}