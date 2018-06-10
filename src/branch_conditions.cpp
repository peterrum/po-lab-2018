#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>

#include "branch_conditions.h"
#include "util.h"

using namespace llvm;
namespace pcpo {

BranchConditions::BranchConditions(std::map<BasicBlock *, State> &programPoints)
    : programPoints(programPoints) {}

bool BranchConditions::isBasicBlockReachable(BasicBlock *pred, BasicBlock *bb) {
  auto bc = branchConditions.find(pred);
  if (bc != branchConditions.end())
    if (bc->second.find(bb) != bc->second.end()) {
      auto &branchConditions_map = bc->second[bb];

      for (auto &branchCondition : branchConditions_map)
        if (branchCondition.second->lessOrEqual(*AD_TYPE::create_bottom()))
          return false;
    }
  return true;
}

void BranchConditions::applyCondition(BasicBlock *pred, BasicBlock *bb) {

  assert(!conditionCacheUsed &&
         "ConditionCache has not been correctly unapplied last time!");

  auto bc = branchConditions.find(pred);
  if (bc != branchConditions.end())
    if (bc->second.find(bb) != bc->second.end()) {
      conditionCacheUsed = true;
      auto &branchConditions_map = bc->second[bb];

      for (auto &branchCondition : branchConditions_map) {
        auto value = branchCondition.first;
        // buffer old value
        conditionCache[value] = programPoints[pred].getAbstractValue(value);
        // overwrite value with condition
        programPoints[pred].vars[value] = branchCondition.second;
      }
    }
}

void BranchConditions::unApplyCondition(BasicBlock *pred) {

  if (!conditionCacheUsed)
    return;

  for (auto &condition : conditionCache)
    programPoints[pred].vars[condition.first] = condition.second;

  conditionCache.clear();
  conditionCacheUsed = false;
}

void BranchConditions::putBranchConditions(BasicBlock *pred, BasicBlock *bb,
                                           Value *val,
                                           std::shared_ptr<AbstractDomain> ad) {

  branchConditions[pred][bb][val] = ad;
}
}
