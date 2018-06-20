#include "state.h"

using namespace llvm;
namespace pcpo {

State::State() : bottom(true) {}

bool State::put(Value &v, std::shared_ptr<AbstractDomain> ad) {

  assert(!bottom && "Visited although bottom!");

  if (ad->isBottom()) {
    DEBUG_OUTPUT("State::put: set to bottom because of " << v.getName());
    ad->printOut();
    bottom = true;
    return true;
  }

  DEBUG_OUTPUT("State::put for " << v.getName());
  if (vars.find(&v) != vars.end()) {
    if (*ad<=(*vars[&v])) {
      return false;
    }

    vars[&v] = vars[&v]->leastUpperBound(*ad);

    if(vars[&v]->requiresWidening()) {
      if(changeCounts[&v] > WIDENING_AFTER) {
          vars[&v] = vars[&v]->widen();
      }
      changeCounts[&v]++;
    }
  } else {
    vars[&v] = ad;
    changeCounts[&v] = 0;
  }

  return true;
}

shared_ptr<AbstractDomain> State::getAbstractValue(Value *v) const {

  if (ConstantInt::classof(v)) {
    return shared_ptr<AbstractDomain>(new AD_TYPE(reinterpret_cast<ConstantInt *>(v)->getValue()));
  }

  const auto find = vars.find(v);
  if (find != vars.end()) {
    return find->second;
  }

  DEBUG_OUTPUT("State::getAbstractValue " << v->getName() << " : failed");

  return AD_TYPE::create_top(v->getType()->getIntegerBitWidth());
}

bool State::isAvailable(Value *v) const {
  if (ConstantInt::classof(v)) {
    return true;
  }

  return vars.find(v) != vars.end();
}

bool State::leastUpperBound(State &other) {
  /// lub of bottom
  bottom = bottom && other.bottom;

  /// case 1: lub(bottom, bottom) = bottom -> definitely no change
  if (isBottom() && other.isBottom())
    return false;

  /// case 2: lub(x,bottom) = x -> definitely no change
  if (other.isBottom())
    return false;

  /// case 3: lub(bottom,y) = y -> definitely change
  if (isBottom())
    return !copyState(other);

  /// case 4: lub(x, y) -> possibly change
  bool change = false;
  for (const auto &var : other.vars)
    change |= put(*var.first, var.second);
  return change;
}

/// is this <= other
bool State::operator<=(State &other) {

  /// bottom <= x
  if (isBottom())
    return true;

  /// x <= bottom
  if (other.isBottom())
    return isBottom();

  /// this.x <= other.x
  for (const auto &var : vars){
    auto find = other.vars.find(var.first);
    if (find != other.vars.end()) {
      if (!(*var.second<=(*find->second))){
        /// ! this.x <= other.x
        return false;
      }
    } else {
        /// x not in other (T in other)
    }
  }
  return true;
}

bool State::copyState(State &other) {
  /// basic block has been visited
  bottom = other.bottom;

  /// copy map
  vars.clear();
  changeCounts.clear();
  for (auto &var : other.vars) {
    vars[var.first] = var.second;
    changeCounts[var.first] = 0;
  }

  return bottom;
}

void State::prune(State &other) {

  assert(!isBottom() && "State::prune: this is bottom!");
  assert(!other.isBottom() && "State::prune: other is bottom!");

  std::map<Value *, std::shared_ptr<AbstractDomain>> temp;

  auto il = vars.begin();
  auto ir = other.vars.begin();

  while (il != vars.end() && ir != other.vars.end()) {
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

bool State::isBottom() const { return bottom; }

void State::markVisited() { bottom = false; }

void State::print() const {
  if (bottom) {
    STD_OUTPUT("bottom");
    return;
  }

  for (const auto &var : vars)
    STD_OUTPUT(var.first->getName() << " -> " << *var.second);
}
}  // namespace pcpo
