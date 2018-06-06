#ifndef PROJECT_VISITOR_H
#define PROJECT_VISITOR_H

#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/InstrTypes.h"
#include <queue>

using namespace llvm;

namespace pcpo{

class Maluba {
/// Do nothing -> RetType
};


class VsaVisitor : public InstVisitor<VsaVisitor,void> {

public:
    VsaVisitor(std::queue<Instruction*>& q):worklist(q) { };

    void visitInstruction(Instruction &I);
    void visitBinaryOperator(BinaryOperator& I);

private:
    std::queue<Instruction*>& worklist;
};

}

#endif //PROJECT_VISITOR_H
