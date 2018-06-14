/*#include <llvm/ADT/Statistic.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Module.h>*/

#include "../../src/util/util.h"
#include "../../src/vsa.h"

#define DEBUG_TYPE "hello"

namespace {

struct VsaBenchmarkPass : public ModulePass {

  static char ID;

  VsaBenchmarkPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {

    /// analysis has made no modifications
    return false;
  }

  /// We don't modify the program, so we preserve all analysis.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<LazyValueInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<VsaPass>();
  }
};
}

static RegisterPass<VsaPass> Y("vsapass", "VSA Pass");

char VsaBenchmarkPass::ID = 0;
static RegisterPass<VsaBenchmarkPass> Z("vsabenchmarkpass", "VSA Benchmark Pass");
