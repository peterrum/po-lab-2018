#include "vsa.h"

#include "api/vsa_result.h"
#include "fixpoint/vsa_visitor.h"
#include "fixpoint/worklist.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>

#include "llvm/IR/Dominators.h"

#define DEBUG_TYPE "hello"

static RegisterPass<VsaPass> Y("vsapass",
                               "VSA Pass (with getAnalysisUsage implemented)");
