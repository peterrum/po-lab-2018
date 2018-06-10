#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include "AbstractDomain.h"
#include "BoundedSet.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <memory>
#include <utility>

using namespace llvm;
namespace pcpo {

class State {

public:
  State();

  /// true -> change, false -> no change
  bool put(Value &v, std::shared_ptr<AbstractDomain> ad);
  /// true -> change, false -> no change (see State::put)
  bool leastUpperBound(State &other);

  bool copyState(State &other);

  /// overwrite state with intersection of this and other state
  /// only working if both states not bottom
  void prune(State &other);

  shared_ptr<AbstractDomain> getAbstractValue(Value *v);

  bool isBasicBlockReachable(BasicBlock *bb);

  void applyCondition(BasicBlock *bb);

  void unApplyCondition();

  bool isBottom();

  void setNotBottom();

  void print();

  void putBranchConditions(BasicBlock *bb, Value *val,
                           std::shared_ptr<AbstractDomain> ad);

  void transferBranchConditions(State &other);

  void transferBottomness(State &other);

private:
  std::map<Value *, std::shared_ptr<AbstractDomain>> vars;
  std::map<BasicBlock *, std::map<Value *, std::shared_ptr<AbstractDomain>>>
      branchConditions;

  std::map<Value *, std::shared_ptr<AbstractDomain>> conditionCache;
  bool conditionCacheUsed;

  bool bottom;
};

} /// namespace

#endif // PROJECT_STATE_H
