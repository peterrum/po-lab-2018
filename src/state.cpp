#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>

#include "state.h"
#include "util.h"

using namespace llvm;
namespace pcpo {

State::State() : conditionCacheUsed(false), bottom(true) {}

bool State::put(Value &v, std::shared_ptr<AbstractDomain> ad) {

  assert(!bottom && "Visited although bottom!");

  if (ad->lessOrEqual(*BoundedSet::create_bottom())) {
    DEBUG_OUTPUT("State::put: set to bottom because of " << v.getName());
    ad->printOut();
    bottom = true;
    return true;
  }

  DEBUG_OUTPUT("State::put for " << v.getName());
  // ad->printOut();
  if (vars.find(&v) != vars.end()) {
    if (ad->lessOrEqual(*vars[&v]))
      return false;
    vars[&v] = vars[&v]->leastUpperBound(*ad);

  } else {
    vars[&v] = ad;
  }
  return true;
}

bool State::leastUpperBound(State &other) {
  /// case 1: lub(bottom, bottom) = bottom -> definitely no change
  if (this->isBottom() && other.isBottom())
    return false;

  // case 2: lub(x,bottom) = x -> definitely no change
  if (other.isBottom())
    return false;

  // case 3: lub(bottom,y) = y -> definitely change
  if (this->isBottom())
    return !this->copyState(other);

  // case 4: lub(x, y) -> possibly change
  bool change = false;
  for (auto &var : other.vars)
    change |= put(*var.first, var.second);
  return change;
}

bool State::copyState(State &other) {
  // basic block has been visited
  bottom = other.bottom;
  // copy map
  vars.clear();
  for (auto &var : other.vars)
    vars[var.first] = var.second;
  return bottom;
}

void State::prune(State &other) {
    
  assert(!this->isBottom() && "State::prune: this is bottom!");
  assert(!other.isBottom() && "State::prune: other is bottom!");
    
  std::map<Value *, std::shared_ptr<AbstractDomain>> temp;

  auto il = this->vars.begin();
  auto ir = other.vars.begin();

  while (il != this->vars.end() && ir != other.vars.end()) {
    if (il->first < ir->first)
      ++il;
    else if (ir->first < il->first)
      ++ir;
    else {
      temp.insert(*il);
      ++il;
      ++ir;
    }
  }

  vars = temp;
}

shared_ptr<AbstractDomain> State::getAbstractValue(Value *v) {

  if (ConstantInt::classof(v)) {
    auto temp = reinterpret_cast<ConstantInt *>(v);
    return shared_ptr<AbstractDomain>(new BoundedSet(temp->getValue()));
  }

  auto find = vars.find(v);
  if (find != vars.end()) {
    return find->second;
  }

  DEBUG_OUTPUT("State::getAbstractValue " << v->getName() << " : failed");

  return AD_TYPE::create_top();
}

bool State::isBasicBlockReachable(BasicBlock *bb) {
  if (branchConditions.find(bb) != branchConditions.end()) {
    auto &branchConditions_map = branchConditions[bb];

    for (auto &branchCondition : branchConditions_map)
      if (branchCondition.second->lessOrEqual(*AD_TYPE::create_bottom()))
        return false;
  }
  return true;
}

void State::applyCondition(BasicBlock *bb) {

  assert(!conditionCacheUsed &&
         "ConditionCache has not been correctly unapplied last time!");

  if (branchConditions.find(bb) != branchConditions.end()) {
    conditionCacheUsed = true;
    auto &branchConditions_map = branchConditions[bb];

    for (auto &branchCondition : branchConditions_map) {
      auto value = branchCondition.first;
      // buffer old value
      conditionCache[value] = getAbstractValue(value);
      // overwrite value with condition
      vars[value] = branchCondition.second;
    }
  }
}

void State::unApplyCondition() {

  if (!conditionCacheUsed)
    return;

  for (auto &condition : conditionCache)
    vars[condition.first] = condition.second;

  conditionCache.clear();
  conditionCacheUsed = false;
}

bool State::isBottom() { return bottom; }

void State::setNotBottom() { bottom = false; }

void State::print() {
  if (bottom) {
    STD_OUTPUT("bottom");
    return;
  }

  for (auto &var : vars) {
    // std::cout << *var.second << std::endl;
    STD_OUTPUT(var.first->getName() << " -> " << *var.second);
    // var.second->printOut();
  }
}

void State::putBranchConditions(BasicBlock *bb, Value *val,
                                std::shared_ptr<AbstractDomain> ad) {

  branchConditions[bb][val] = ad;
}

void State::transferBranchConditions(State &other) {
  this->branchConditions = other.branchConditions;
}

void State::transferBottomness(State &other) { this->bottom = other.bottom; }
}
