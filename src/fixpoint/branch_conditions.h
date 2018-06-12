#ifndef PROJECT_BRANCH_CONDITIONS_H
#define PROJECT_BRANCH_CONDITIONS_H

#include "AbstractDomain.h"
#include "BoundedSet.h"
#include "state.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <memory>
#include <utility>

using namespace llvm;
namespace pcpo {

class BranchConditions {

public:
  BranchConditions(std::map<BasicBlock *, State> &programPoints);

  bool isBasicBlockReachable(BasicBlock *pred, BasicBlock *bb);

  void applyCondition(BasicBlock *pred, BasicBlock *bb);

  void unApplyCondition(BasicBlock *pred);

  void putBranchConditions(BasicBlock *pred, BasicBlock *bb, Value *val,
                           std::shared_ptr<AbstractDomain> ad);

private:
  std::map<BasicBlock *, State> &programPoints;

  std::map<BasicBlock *,
           std::map<BasicBlock *,
                    std::map<Value *, std::shared_ptr<AbstractDomain>>>>
      branchConditions;

  std::map<Value *, std::shared_ptr<AbstractDomain>> conditionCache;
  bool conditionCacheUsed;
};

} /// namespace

#endif