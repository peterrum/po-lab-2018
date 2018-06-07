#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "vsa_visitor.h"
#include <queue>

using namespace llvm;
using namespace pcpo;

#define DEBUG_TYPE "hello"

namespace {
    
  struct VsaPass : public ModulePass {
    // Pass identification, replacement for typeid
    static char ID; 
    
    // worklist: instructions are handled in a FIFO manner
    std::queue<Instruction*> worklist;
    
    // visitor: visits instructions and pushes new instructions onto the
    // worklist
    VsaVisitor vis;
    
    VsaPass() : ModulePass(ID), worklist(), vis(worklist) {}

    bool doInitialization(Module& m) override {
        return ModulePass::doInitialization(m);
    }
    
    bool doFinalization(Module & m) override{ 
        return ModulePass::doFinalization(m);
    }
    
    bool runOnModule(Module &M) override {
      // push all instructions onto the worklist: for that loop over...
      // ... all functions
      for(auto& function: M)
          // ... basic blocks
          for(auto& bb : function)
              // and instructions
              for(auto& instr:bb)
                  worklist.push(&instr);
          
      // pop instructions from the worklist and visit them until no more
      // are available (the visitor pushes new instructions query-based)
      while(!worklist.empty()){
          vis.visit(*worklist.front());
          worklist.pop();
      }

      // TODO: purpose?
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char VsaPass::ID = 0;
static RegisterPass<VsaPass> Y("vsapass", "VSA Pass (with getAnalysisUsage implemented)");
