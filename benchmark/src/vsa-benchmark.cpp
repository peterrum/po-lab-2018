/*#include <llvm/ADT/Statistic.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Module.h>*/

#include <llvm/Analysis/LazyValueInfo.h>
#include <llvm/Pass.h>
#include <llvm/IR/Dominators.h>
#include "llvm/IR/ConstantRange.h"

#include "../../src/util/util.h"
#include "../../src/vsa.h"
//using namespace llvm;
namespace {

struct VsaBenchmarkPass : public ModulePass {

  static char ID;

  VsaBenchmarkPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {

      uint64_t vsa_count = 0;
      uint64_t lvi_count = 0;
      uint64_t eq_count = 0;

      /// extract results from VsaPass
      const auto& vsa = getAnalysis<VsaPass>().getResult();

      for (auto &f : M.functions()) {
          /// check if function is empty
          if(f.begin()==f.end())
              continue;

          /// get LazyValueInfo for function
          LazyValueInfo& lvi = getAnalysis<LazyValueInfoWrapperPass>(f).getLVI();

          for (auto& bb : f) {
            /// block not reachable in vsa -> vsa.value = bottom
            if (!vsa.isReachable(&bb)){
              for (auto &instr: bb) {
                  if(!instr.getType()->isIntegerTy()) continue;
                  const llvm::ConstantRange lvi_const_range = lvi.getConstantRange(&instr, &bb);
                  /// extract results of variable i
                  if(lvi_const_range.isEmptySet()) eq_count++; /// both know it is bottom
                  else vsa_count++; /// lvi thinks it's not bottom
              }
              continue;
            }

            for (auto &instr: bb) {
              if(!instr.getType()->isIntegerTy()) continue;
              const llvm::ConstantRange lvi_const_range = lvi.getConstantRange(&instr, &bb);

              /// no results in vsa -> vsa.value = top
              if(!vsa.isResultAvailable(&bb, &instr)) {
                if (lvi_const_range.isFullSet()) eq_count++; /// both T
                else lvi_count++; /// lvi has something better
                continue;
              }

              const auto abstractValue = vsa.getAbstractValue(&bb, &instr);
              if(abstractValue->isTop()){
                  if(lvi_const_range.isFullSet()) eq_count++; /// both T
                  else lvi_count++;  /// lvi has something better
                  continue;
              }

              /// Comparing set sizes
              const auto vsa_size = abstractValue->getNumValues().getZExtValue();
              const auto lvi_size = lvi_const_range.getSetSize().getZExtValue();

              if(vsa_size == lvi_size) eq_count++; // both same size
              else if(vsa_size < lvi_size) vsa_count++; // vsa smaller -> better
              else lvi_count++; // lvi smaller -> better
          }
        }
      }

    errs() << vsa_count << "," << lvi_count << "," << eq_count << '\n';

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
