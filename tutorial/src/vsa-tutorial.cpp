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

      STD_OUTPUT("\n----------------------------------");
      STD_OUTPUT("Function: " << f.getName() << ":");
      STD_OUTPUT("----------------------------------");

      // get LazyValueInfo for function
      LazyValueInfo& lvi = getAnalysis<LazyValueInfoWrapperPass>(f).getLVI();

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
            if(res == 1)
                STD_OUTPUT("VSA (is " << i.getName() << " < 12 ?)" << " yes");
            else if (res == 0)
                STD_OUTPUT("VSA (is " << i.getName() << " < 12 ?)" << " no");
            else
                STD_OUTPUT("VSA (is " << i.getName() << " < 12 ?)" << " maybe");

            // compare with LVI:
            auto res2 = lvi.getPredicateAt(CmpInst::Predicate::ICMP_ULT, &i, v1, &i);
            if(res2 == 1)
                STD_OUTPUT("LVI (is " << i.getName() << " < 12 ?)" << " yes");
            else if (res2 == 0)
                STD_OUTPUT("LVI (is " << i.getName() << " < 12 ?)" << " no");
            else
                STD_OUTPUT("LVI (is " << i.getName() << " < 12 ?)" << " maybe");

            // new line
            STD_OUTPUT("");

            if(abstractValue->isTop()) {
              STD_OUTPUT("----------------------------------");
              continue;
            }

            auto size = abstractValue->getNumValues().getZExtValue();
            //STD_OUTPUT("AD getNumValues: '" << size << "'");

            if(size == 0){
              STD_OUTPUT("----------------------------------");
              continue;
            }

            STD_OUTPUT("VSA " << *abstractValue);

            //for(uint64_t i=0; i<size;i++)
            //    STD_OUTPUT("AD getValue: '" << abstractValue->getValueAt(i).getZExtValue() << "'");

            //if(abstractValue->isConstant())
            //    STD_OUTPUT("AD getConstant if constant: '" << abstractValue->getConstant() << "'");

            // do the same for LazyValueInfo
            //auto lvi_const = lvi.getConstant(&i, &b);
            //if(!(lvi_const==nullptr))
            //    STD_OUTPUT("LazyValueInfo getConstant: '" << lvi_const->getUniqueInteger() << "'");

            auto lvi_const_range = lvi.getConstantRange(&i, &b);
            STD_OUTPUT("LVI " << lvi_const_range);
/*

            STD_OUTPUT("AD UMin: '" << abstractValue->getUMin() << "'");
            STD_OUTPUT("AD SMin: '" << abstractValue->getSMin() << "'");
            STD_OUTPUT("AD UMax: '" << abstractValue->getUMax() << "'");
            STD_OUTPUT("AD SMax: '" << abstractValue->getSMax() << "'");
*/

            STD_OUTPUT("----------------------------------");
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
