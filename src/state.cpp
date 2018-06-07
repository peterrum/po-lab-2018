#include "state.h"

bool State::put(Value& v, std::shared_ptr<AbstractDomain> ad){
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
    for(auto& var : vars){
        change |= put(*var.first,var.second);
    }
    return change;
}

shared_ptr<AbstractDomain> State::getAbstractValues(Value* v){
    errs() << "entering abstractvalues\n";
    auto find = vars.find(v);
    if(find != vars.end()){
        errs() << "found\n";
        return find->second;
    }
    errs() << " not found\n";
    return shared_ptr<AbstractDomain>(new BoundedSet(true));
}

std::string State::toString(){
    return std::to_string(vars.size());
}

