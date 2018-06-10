#include "vsa_visitor.h"
#include "worklist.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>

using namespace llvm;
using namespace pcpo;

#define DEBUG_TYPE "hello"

namespace {

struct VsaPass : public ModulePass {
  // Pass identification, replacement for typeid
  static char ID;

  // worklist: instructions are handled in a FIFO manner
  WorkList worklist;

  // visitor: visits instructions and pushes new instructions onto the
  // worklist
  VsaVisitor vis;

  VsaPass() : ModulePass(ID), worklist(), vis(worklist) {}

  bool doInitialization(Module &m) override {
    return ModulePass::doInitialization(m);
  }

  bool doFinalization(Module &m) override {
    return ModulePass::doFinalization(m);
  }

  bool runOnModule(Module &M) override {
    /// loop over all functions in the module
    for (auto &function : M) {
      /// get the first basic block and push it into the worklist
      for (auto &bb : function) {
        worklist.push(&bb);
        break;
      }
      int visits = 0;

      std::map<std::string, std::vector<int>> trance;
      for (auto &bb : function)
        trance[std::string(bb.getName())].clear();

      // pop instructions from the worklist and visit them until no more
      // are available (the visitor pushes new instructions query-based)
      while (!worklist.empty()) {

        trance[std::string(worklist.peek()->getName())].push_back(visits);

        vis.visit(*worklist.pop());

        DEBUG_OUTPUT("");
        DEBUG_OUTPUT("Global state after " << visits << " visits");
        vis.print();
        DEBUG_OUTPUT("");
        DEBUG_OUTPUT("");
        DEBUG_OUTPUT("");
        DEBUG_OUTPUT("");

        visits++;
      }

      /// print trance
      errs() << "\nTRACE OF FUNCTION " << function.getName() << ":\n";
      for (auto t : trance) {
        errs() << t.first << "#" << t.second.size() << ": ";
        for (auto s : t.second)
          errs() << s << " ";
        errs() << "\n";
      }
    } // go to next function

    // Our analysis does not change the IR
    return false;
  }

  // We don't modify the program, so we preserve all analyses.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};
}

char VsaPass::ID = 0;
static RegisterPass<VsaPass> Y("vsapass",
                               "VSA Pass (with getAnalysisUsage implemented)");
