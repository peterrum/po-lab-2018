#ifndef PROJECT_VISITOR_H
#define PROJECT_VISITOR_H

#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/InstrTypes.h"
#include <queue>

using namespace llvm;

namespace pcpo{

class VsaVisitor : public InstVisitor<VsaVisitor,void> {

public:
    VsaVisitor(std::queue<Instruction*>& q):worklist(q) { };

    void visitInstruction(Instruction &I);
    void visitPHINode(PHINode &I);
    void visitBinaryOperator(BinaryOperator& I);

private:
    void pushInstUsers(Instruction &I);

    std::queue<Instruction*>& worklist;
};

}

#endif //PROJECT_VISITOR_H
