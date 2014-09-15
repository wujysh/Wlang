#include <stack>
#include <typeinfo>
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Analysis/FindUsedTypes.h"
#include "llvm/Analysis/IntervalPartition.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Vectorize.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/Support/MemoryBuffer.h"

using namespace llvm;

class NProgram;

class CodeGenBlock {
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
public:
    std::vector<CodeGenBlock *> blocks;
    Function *mainFunction;
    Module *module;
    CodeGenContext(): module(new Module("main", getGlobalContext())) { }

    void runLLVMOptimizations();
    void generateCode(NProgram& root);
    GenericValue runCode();
    std::map<std::string, Value*>& locals() { return blocks.back()->locals; }
    BasicBlock *currentBlock() { return blocks.back()->block; }
    Module &getModule() { return *module; }
    void pushBlock(BasicBlock *block) { blocks.push_back(new CodeGenBlock()); blocks.back()->block = block; }
    void popBlock() { CodeGenBlock *top = blocks.back(); blocks.pop_back(); if (top != nullptr) delete top; }
};


void llvmerror(char const *, int&, int&, int&);