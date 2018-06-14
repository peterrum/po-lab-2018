#include "llvm/ADT/Statistic.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/ConstantRange.h>
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

    // extract results from VsaPass
    auto &results = getAnalysis<VsaPass>().getResult();

    // print states of all basic blocks of this module
    results.print();

    // iterate such that we get a block...
    for (auto &f : M.functions()) {

      // check if function is empty
      if(f.begin()==f.end())
        continue;

      errs() << "\n----------------------------------\n";
      errs() << "Function: " << f.getName() << ":\n";
      errs() << "----------------------------------\n";

      for (auto &b : f) {
        // is block reachable
        if (!results.isReachable(&b))
          continue; // no

        for(auto &i: b) {
            // are results regarding the variable v1 available
            if (!results.isResultAvailable(&b, &i))
              continue; // no

            errs() << i.getName() << "\n";

            // extract results of variable i
            auto abstractValue = results.getAbstractValue(&b, &i);

            // create a constant with value 12
            ConstantInt *const12= ConstantInt::get(M.getContext(), APInt(i.getType()->getIntegerBitWidth(), 12));

            // perform trivial comparison (unsigned less than) between value and v1
            // more info in: llvm/IR/InstrTypes.h
            auto res = abstractValue->testIf(CmpInst::Predicate::ICMP_ULT, const12);
            if(res == 1)
                errs() << "is " << i.getName() << " < 12 ?" << " yes";
            else if (res == 0)
                errs() << "is " << i.getName() << " < 12 ?" << " no";
            else
                errs() << "is " << i.getName() << " < 12 ?" << " maybe";

            // new line
            errs() << "\n\n";

            if(abstractValue->isTop()) {
              errs() << "----------------------------------\n";
              continue;
            }

            auto size = abstractValue->getNumValues().getZExtValue();
            errs() << "AD size: " << size << "\n";

            if(size == 0){
              errs() << "----------------------------------\n";
              continue;
            }

            errs() << "Value set " << *abstractValue << "\n";

            if(abstractValue->isConstant())
                errs() << "AD is constant: " << abstractValue->getConstant() << "\n";


            errs() << "AD Min: u " << abstractValue->getUMin() << ", ";
            errs() << "s " << abstractValue->getSMin() << "\n";

            errs() << "AD Max: u " << abstractValue->getUMax() << ", ";
            errs() << "s " << abstractValue->getSMax() << "\n";

            errs() << "----------------------------------\n";
        }
      }
    }

    // analysis has made no modifications
    return false;
  }

  // We don't modify the program, so we preserve all analysis.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<LazyValueInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<VsaPass>();
  }
};
}

static RegisterPass<VsaPass> Y("vsapass", "VSA Pass");

char VsaTutorialPass::ID = 0;
static RegisterPass<VsaTutorialPass> Z("vsatutorialpass", "VSA Tutorial Pass");
