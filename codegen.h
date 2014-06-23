#include <stack>
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/IR/IRBuilder.h"
//#include "llvm/IR/ModuleProvider.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/Support/raw_ostream.h"
#include <typeinfo>

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

    void generateCode(NProgram& root);
    GenericValue runCode();
    std::map<std::string, Value*>& locals() { return blocks.back()->locals; }
    BasicBlock *currentBlock() { return blocks.back()->block; }
    void pushBlock(BasicBlock *block) { blocks.push_back(new CodeGenBlock()); blocks.back()->block = block; }
    void popBlock() { CodeGenBlock *top = blocks.back(); blocks.pop_back(); if (top != nullptr) delete top; }
};
