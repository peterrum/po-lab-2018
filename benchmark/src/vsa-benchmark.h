#ifndef PROJECT_VSA_BENCHMARK_H
#define PROJECT_VSA_BENCHMARK_H

#include <llvm/Analysis/LazyValueInfo.h>
#include <llvm/Pass.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/ConstantRange.h>
#include "../../src/util/util.h"
#include "../../src/vsa.h"

namespace {

struct VsaBenchmarkPass : public ModulePass {

    static char ID;

    VsaBenchmarkPass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override;

    /// We don't modify the program, so we preserve all analysis.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.setPreservesAll();
        AU.addRequired<LazyValueInfoWrapperPass>();
        AU.addRequired<DominatorTreeWrapperPass>();
        AU.addRequired<VsaPass>();
    }
};

static RegisterPass<VsaPass> Y("vsapass", "VSA Pass");

char VsaBenchmarkPass::ID = 0;
static RegisterPass<VsaBenchmarkPass> Z("vsabenchmarkpass", "VSA Benchmark Pass");

} //namespace


#endif //PROJECT_VSA_BENCHMARK_H
