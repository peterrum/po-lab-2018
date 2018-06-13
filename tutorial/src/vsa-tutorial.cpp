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

    results.print();

    // iterate such that we get a block...
    for (auto &f : M.functions()) {
      for (auto &b : f) {

        // is block reachable
        if (!results.isReachable(&b))
          continue; // no

        for(auto &i: b) {
            // are results regarding the variable v1 available
            if (!results.isResultAvailable(&b, &i))
              continue; // no

            // extract results of variable i
            auto abstractValue = results.getAbstractValue(&b, &i);

            // create a constant with value 12
            ConstantInt *v1 = ConstantInt::get(M.getContext(), APInt(i.getType()->getIntegerBitWidth(), 12));

            // perform trivial comparison (unsigned less than) between value and v1
            // more info in: llvm/IR/InstrTypes.h
            auto res = abstractValue->testIf(CmpInst::Predicate::ICMP_ULT, v1);
            if(res)
                STD_OUTPUT(i.getName() << " a <  b" << v1->getName());
            else
                STD_OUTPUT(i.getName() << " a >= b" << v1->getName());


            if(abstractValue->isTop()) {
              continue;
            }

            auto size = abstractValue->getNumValues().getZExtValue();
            STD_OUTPUT("AD getNumValues: '" << size << "'");

            if(size == 0)
              continue;

            for(uint64_t i=0; i<size;i++)
                STD_OUTPUT("AD getValue: '" << abstractValue->getValueAt(i).getZExtValue() << "'");

            if(abstractValue->isConstant())
                STD_OUTPUT("AD getConstant if constant: '" << abstractValue->getConstant() << "'");

              STD_OUTPUT("AD UMin: '" << abstractValue->getUMin() << "'");
              STD_OUTPUT("AD SMin: '" << abstractValue->getSMin() << "'");
              STD_OUTPUT("AD UMax: '" << abstractValue->getUMax() << "'");
              STD_OUTPUT("AD SMax: '" << abstractValue->getSMax() << "'");


              STD_OUTPUT("----------------------------------");
        }
        STD_OUTPUT("----------------------------------");
      }
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
