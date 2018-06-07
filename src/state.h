#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include <map>

using namespace llvm;
namespace pcpo {

class State {

public:
    State() = default;

    /// true -> change, false -> no change
    bool put(Value &v, std::shared_ptr <AbstractDomain> ad);
    /// true -> change, false -> no change (see State::put)
    bool leastUpperBound(State &other);

    shared_ptr<AbstractDomain> getAbstractValues(Value* v);

private:
    std::map<Value *, std::shared_ptr < AbstractDomain>> vars;
};

} /// namespace

#endif //PROJECT_STATE_H
