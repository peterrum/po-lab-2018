#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>
#include <llvm/IR/Module.h>
#include <llvm/IR/InstrTypes.h>

#include "llvm/IR/Dominators.h"

#include "../../src/vsa.cpp"
#include "../../src/util/util.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"

using namespace llvm;

#define DEBUG_TYPE "hello"

namespace {

struct VsaTutorialPass : public ModulePass {
    
  static char ID;

  VsaTutorialPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
      
    VsaPass pass;
    
    pass.runOnModule(M);
    
    auto& results = pass.result;
    for(auto & f : M.functions())
        for(auto & b : f){
            
            ConstantInt * v1 = ConstantInt::get(M.getContext(), APInt(64, 12));
            
            auto temp = results.getAbstractValue(&b, v1);
            
            {
                ConstantInt * v2 = ConstantInt::get(M.getContext(), APInt(64, 12));
                auto res = temp->testIf(CmpInst::Predicate::ICMP_ULT, v2);
                STD_OUTPUT(b.getName() << " " << res);
            }
            
            {
                ConstantInt * v2 = ConstantInt::get(M.getContext(), APInt(64, 18));
                auto res = temp->testIf(CmpInst::Predicate::ICMP_ULT, v2);
                STD_OUTPUT(b.getName() << " " << res);
            }
            
            {
                ConstantInt * v2 = ConstantInt::get(M.getContext(), APInt(64, 6));
                auto res = temp->testIf(CmpInst::Predicate::ICMP_ULT, v2);
                STD_OUTPUT(b.getName() << " " << res);
            }
            
            break;
        }
    
    return false;
  }

  // We don't modify the program, so we preserve all analyses.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};
}

char VsaTutorialPass::ID = 0;
static RegisterPass<VsaTutorialPass> Z("vsatutorialpass",
                               "VSA Tutorial Pass");
