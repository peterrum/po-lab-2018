#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>

#include "util.h"
#include "state.h"

using namespace llvm;
namespace pcpo {

bool State::put(Value& v, std::shared_ptr<AbstractDomain> ad){
    
    DEBUG_OUTPUT("State::put for " << v.getName());
    //ad->printOut();
    if(vars.find(&v) != vars.end()){
        if(ad->lessOrEqual(*vars[&v]))
            return false;
        vars[&v] = vars[&v]->leastUpperBound(*ad);

    } else {
        vars[&v] = ad;
    }
    return true;
}

bool State::leastUpperBound(State& other){
    bool change = false;
    for(auto& var : other.vars){
        change |= put(*var.first,var.second);
    }
    return change;
}

shared_ptr<AbstractDomain> State::getAbstractValues(Value* v){
    
    if(ConstantInt::classof(v)){
        auto temp = reinterpret_cast<ConstantInt*>(v);
        return shared_ptr<AbstractDomain>(new BoundedSet(temp->getValue()));
    }
    
    auto find = vars.find(v);
    if(find != vars.end()){
        return find->second;
    }
    
    
    DEBUG_OUTPUT( "State::getAbstractValues " << v->getName() <<" : failed" );
    
    return BoundedSet::create_top();
}

}