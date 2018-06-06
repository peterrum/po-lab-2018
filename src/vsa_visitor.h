#ifndef PROJECT_VISITOR_H
#define PROJECT_VISITOR_H

#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/InstrTypes.h"
#include "AbstractDomain.h"
#include <queue>
#include <unordered_map>
#include <map>

using namespace llvm;

namespace pcpo{

class State{

public:
    State() = default;

    bool put(Value& v, std::shared_ptr<AbstractDomain> ad);
    void leastUpperBound(State& other);

private:
    std::map<Value*,std::shared_ptr<AbstractDomain>> vars;
};

class VsaVisitor : public InstVisitor<VsaVisitor,void> {

public:
    VsaVisitor(std::queue<Instruction*>& q):worklist(q) { };

    /// Specific Instruction type classes
    /*void visitBranchInst(BranchInst &I);
    void visitSwitchInst(SwitchInst &I);
    void visitIndirectBrInst(IndirectBrInst &I);
    void visitResumeInst(ResumeInst &I);
    void visitICmpInst(ICmpInst &I);
    void visitLoadInst(LoadInst &I);
    void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I);*/
    void visitPHINode(PHINode &I);
    /*void visitTruncInst(TruncInst &I);
    void visitZExtInst(ZExtInst &I);
    void visitSExtInst(SExtInst &I);
    void visitBitCastInst(BitCastInst &I);
    void visitSelectInst(SelectInst &I);
    void visitVAArgInst(VAArgInst &I);
    void visitExtractElementInst(ExtractElementInst &I);
    void visitExtractValueInst(ExtractValueInst &I);*/

    /// Call and Invoke
    //void visitCallInst(CallInst &I);
    //void visitInvokeInst(InvokeInst &I);

    /// if not specific get the whole class
    //void visitCastInst(CastInst &I);
    void visitBinaryOperator(BinaryOperator& I);
    //void visitCmpInst(CmpInst &I);
    void visitUnaryInstruction(UnaryInstruction &I);

    /// default
    void visitInstruction(Instruction &I);

private:
    void pushInstUsers(Instruction &I);

    std::queue<Instruction*>& worklist;
    std::map<BasicBlock,State> programPoints;
};

}

#endif //PROJECT_VISITOR_H
