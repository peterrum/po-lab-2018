#include "llvm/ADT/Statistic.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Module.h>

#include <queue>

#include "../../src/util/util.h"
#include "../../src/vsa.h"

using namespace llvm;

#define DEBUG_TYPE "hello"

namespace {

struct VsaTutorialPass : public ModulePass {

  static char ID;

  VsaTutorialPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {

    // run vsa-pass
    ::VsaPass pass(false);
    pass.runOnModule(M);

    // extract results
    auto &results = pass.result;

    // iterate such that we get a block...
    for (auto &f : M.functions())
      for (auto &b : f) {

        // is block reachable
        if (!results.isReachable(&b))
          continue; // no

        // create a constant with value 12
        ConstantInt *v1 = ConstantInt::get(M.getContext(), APInt(64, 12));
        // ... with value
        ConstantInt *v2 = ConstantInt::get(M.getContext(), APInt(64, 18));
        
        // are results regarding the variable v1 available
        if (!results.isResultAvailable(&b, v1))
          continue; // no

        // extract results of variable v1
        auto temp = results.getAbstractValue(&b, v1);

        // perform trivial comparison (unsigned less than) between v1 and v2
        // possible ICMP_EQ, _NE, _UGT, _UGE, _ULT, _ULE, _SGT, _SGE, _SLT, _SLE
        // more info in: llvm/IR/InstrTypes.h
        auto res = temp->testIf(CmpInst::Predicate::ICMP_ULT, v2);
        if(res)
            STD_OUTPUT(v1->getName() << "a <  b" << v2->getName());
        else
            STD_OUTPUT(v1->getName() << "a >= b" << v2->getName());

        // this is only a test: so we can stop now!
        break;
      }

    // analysis has made no modifications
    return false;
  }

  // We don't modify the program, so we preserve all analysis.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};
}

char VsaTutorialPass::ID = 0;
static RegisterPass<VsaTutorialPass> Z("vsatutorialpass", "VSA Tutorial Pass");
