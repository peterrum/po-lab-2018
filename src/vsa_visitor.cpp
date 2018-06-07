#include "vsa_visitor.h"

using namespace llvm;

namespace pcpo {


bool State::put(Value& v, std::shared_ptr<AbstractDomain> ad){
    const auto find = vars.find(&v);
    if(find != vars.end()){
        if(ad->lessOrEqual(*vars[&v]))
            return false;
        vars[&v] = vars[&v]->leastUpperBound(*ad);

    } else {
        vars[&v] = ad;
    }
    return true;
}

void State::leastUpperBound(State& other){
    for(auto& var : vars){
        put(*var.first,var.second);
    }
}

void VsaVisitor::visitPHINode(PHINode &I) {
    /// todo: merge
    if(true){
        this->pushInstUsers(I);
    }
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {
    // todo
    errs() << "visited binary Operator"<<"\n";
    if(true)
        this->pushInstUsers(I);
}

void VsaVisitor::visitUnaryInstruction(UnaryInstruction &I) {
    // todo
    if(true)
        this->pushInstUsers(I);
}

void VsaVisitor::visitInstruction(Instruction &I){
    // todo: top or exception
    errs() << I.getOpcodeName() << ": " << I.getValueID() << "\n";
}

void VsaVisitor::pushInstUsers(Instruction &I) {
    for(auto values: I.users()){
        if(Instruction::classof(values)) {
            Instruction *v = reinterpret_cast<Instruction *>(values);
            worklist.push(v);
        }
    }
}

}

