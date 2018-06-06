//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "vsa_visitor.h"
#include <queue>

using namespace llvm;
using namespace pcpo;

#define DEBUG_TYPE "hello"

//STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // Hello2 - The second implementation with getAnalysisUsage implemented.
  struct VsaPass : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
      VsaPass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      std::queue<Instruction*> worklist;
      VsaVisitor vis(worklist);
      //vis.visit(M);

      for(auto& function: M){
          //errs().write_escaped(function.getName()) << '\n';
          for(auto& bb : function){
              //errs()<< "BB\n";
              for(auto& instr:bb){
                  //errs() << instr.getOpcodeName() << "\n";
                  //const Instruction* in = &instr;
                  worklist.push(&instr);
              }
          }
      }



      while(!worklist.empty()){
          vis.visit(*worklist.front());
          //errs() << worklist.front()->getOpcodeName() << "\n";
          worklist.pop();
          //errs() << instr->getOpcodeName() << "\n";
      }

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
