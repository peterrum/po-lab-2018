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
  /// constructor: bottom
  State();

  /// set abstract domain of variable: true -> change, false -> no change
  bool put(Value &v, std::shared_ptr<AbstractDomain> ad);

  /// get abstract domain of variable
  shared_ptr<AbstractDomain> getAbstractValue(Value *v);

  /// return if a global sate for this variable is available
  bool isAvailable(Value *v);

  /// create least upper bound (lub) in place
  /// true -> change, false -> no change (see State::put)
  bool leastUpperBound(State &other);

  /// make deep copy of state
  bool copyState(State &other);

  /// overwrite state with intersection of this and other state
  /// only working if both states not bottom
  void prune(State &other);

  /// is state bottom, i.e.: is never reached
  bool isBottom();

  /// mark state has been visited, i.e.: set bottom to false
  void markVisited();

  /// print abstract domain of each variable
  void print();

  std::map<Value *, std::shared_ptr<AbstractDomain>> vars;

private:
  bool bottom;
};

} /// namespace

#endif // PROJECT_STATE_H
