#include "vsa_visitor.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitInstruction(Instruction &I){
    errs() << I.getOpcodeName() << ": " << I.getValueID() << "\n";
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {

    errs() << "visited binary Operator"<<"\n";

    /// todo
    if(true){
        for(auto values: I.users()){
            if(Instruction::classof(values)) {
                Instruction *v = reinterpret_cast<Instruction *>(values);
                worklist.push(v);
            }
        }
    }
}


}

