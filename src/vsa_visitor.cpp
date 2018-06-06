#include "vsa_visitor.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitInstruction(Instruction &I){
    errs() << I.getOpcodeName() << "\n";
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {
    Maluba maluba;
    errs() << "visited binary Operator \n";
    return;

}


}

