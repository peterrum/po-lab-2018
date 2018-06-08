#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include <map>
#include <memory>
#include "AbstractDomain.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "BoundedSet.h"
#include <utility>

using namespace llvm;
namespace pcpo {

class State {

public:
    State();

    /// true -> change, false -> no change
    bool put(Value &v, std::shared_ptr <AbstractDomain> ad);
    /// true -> change, false -> no change (see State::put)
    bool leastUpperBound(State &other);

    shared_ptr<AbstractDomain> getAbstractValue(Value* v);

    std::pair<Value *, std::shared_ptr < AbstractDomain>> get_branch_condition(BasicBlock* bb);
    
    void applyCondition(BasicBlock* bb);
    
    void unApplyCondition();
    
    bool isBottom();
    
    void setNotBottom();
    
    
private:
    std::map<Value *, std::shared_ptr < AbstractDomain>> vars;
    std::map<BasicBlock*, std::pair<Value *, std::shared_ptr < AbstractDomain>>> 
            branchConditions;
    
    std::pair<Value *, std::shared_ptr < AbstractDomain>> conditionCache;
    bool conditionCacheUsed;
    
    bool bottom;
};

} /// namespace

#endif //PROJECT_STATE_H
