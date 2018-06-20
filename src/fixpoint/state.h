#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "../abstract_domain/AbstractDomain.h"
#include "../util/util.h"

#include <map>
#include <memory>

using namespace llvm;
namespace pcpo {

class State {
    friend class BranchConditions;

public:
  /// constructor: bottom
  State();

  /// set abstract domain of variable: true -> change, false -> no change
  bool put(Value &v, std::shared_ptr<AbstractDomain> ad);

  /// get abstract domain of variable
  shared_ptr<AbstractDomain> getAbstractValue(Value *v) const;

  /// return if a global sate for this variable is available
  bool isAvailable(Value *v) const;

  /// create least upper bound (lub) in place
  /// true -> change, false -> no change (see State::put)
  bool leastUpperBound(State &other);

  /// is other less or equal to this
  bool operator<=(State &other);

  /// make deep copy of state
  bool copyState(State &other);

  /// overwrite state with intersection of this and other state
  /// only working if both states not bottom
  void prune(State &other);

  /// is state bottom, i.e.: is never reached
  bool isBottom() const;

  /// mark state has been visited, i.e.: set bottom to false
  void markVisited();

  /// print abstract domain of each variable
  void print() const;

private:
  std::map<Value *, std::shared_ptr<AbstractDomain>> vars;

  /// How often has this value been updated (needed for widening)
  /// zero if AD currenly used for this value does not use widening
  std::map<Value *, int> changeCounts;

  bool bottom;
};

} // namespace pcpo

#endif // PROJECT_STATE_H
