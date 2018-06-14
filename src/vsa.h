#ifndef VSA
#define VSA

#include "api/vsa_result.h"
#include "fixpoint/vsa_visitor.h"
#include "fixpoint/worklist.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>

#include "llvm/IR/Dominators.h"

using namespace llvm;
using namespace pcpo;

namespace {

struct VsaPass : public ModulePass {
  // Pass identification, replacement for typeid
  static char ID;

  bool do_print;

  // worklist: instructions are handled in a FIFO manner
  WorkList worklist;

  // global map of programPoints
  std::map<BasicBlock *, State> globalProgramPoints;

private:
  VsaResult result;
public:
    
  VsaPass(bool do_print = true)
      : ModulePass(ID), do_print(do_print), worklist(),
        result(globalProgramPoints) {}

  bool doInitialization(Module &m) override {
    return ModulePass::doInitialization(m);
  }

  bool doFinalization(Module &m) override {
    return ModulePass::doFinalization(m);
  }

  bool runOnModule(Module &M) override {
    /// loop over all functions in the module
    for (auto &function : M) {
      /// ignore empty functions and go to the next function
      if (function.empty())
        continue;

      VsaVisitor vis(worklist, function);

      /// get the first basic block and push it into the worklist
      worklist.push(&function.front());

      int visits = 0;

      std::map<std::string, std::vector<int>> trance;
      for (auto &bb : function)
        trance[std::string(bb.getName())].clear();

      // pop instructions from the worklist and visit them until no more
      // are available (the visitor pushes new instructions query-based)
      while (!worklist.empty()) {

        trance[std::string(worklist.peek()->getName())].push_back(visits);

        vis.visit(*worklist.pop());
#ifdef DEBUG
        print_local(vis, visits);
#endif
        visits++;
      }


      globalProgramPoints.insert(vis.getProgramPoints().begin(),
                                 vis.getProgramPoints().end());

#ifndef DEBUG
      print_local(vis, visits - 1);
#endif

      /// print trance
      if (do_print) {
        errs() << "\nTRACE OF FUNCTION " << function.getName() << ":\n";
        for (auto t : trance) {
          errs() << t.first << "#" << t.second.size() << ": ";
          for (auto s : t.second)
            errs() << s << " ";
          errs() << "\n";
        }
      }
    } // go to next function

    // Our analysis does not change the IR
    return false;
  }

  void print_local(VsaVisitor &vis, int visits) {
    if (!do_print)
      return;
    STD_OUTPUT("");
    STD_OUTPUT("Global state after " << visits << " visits");
    vis.print();
    STD_OUTPUT("");
    STD_OUTPUT("");
    STD_OUTPUT("");
    STD_OUTPUT("");
  }

  // We don't modify the program, so we preserve all analyses.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
  
  VsaResult& getResult(){
      return result;
  }
};

char VsaPass::ID = 0;
}

#endif
