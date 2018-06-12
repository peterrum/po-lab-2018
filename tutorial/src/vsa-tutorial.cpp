#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>

#include "llvm/IR/Dominators.h"

using namespace llvm;

#define DEBUG_TYPE "hello"

namespace {

struct VsaTutorialPass : public ModulePass {
    
  static char ID;

  VsaTutorialPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    return false;
  }

  // We don't modify the program, so we preserve all analyses.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};
}

char VsaTutorialPass::ID = 0;
static RegisterPass<VsaTutorialPass> Y("vsatutorialpass",
                               "VSA Tutorial Pass");
