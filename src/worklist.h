#ifndef WORKLIST
#define WORKLIST

#include "llvm/IR/BasicBlock.h"
#include <queue>
#include <set>

using namespace llvm;

namespace pcpo {

class WorkList {

public:
  void push(BasicBlock *bb);

  BasicBlock *peek();

  BasicBlock *pop();

  bool empty();

private:
  std::queue<BasicBlock *> worklist;
  std::set<BasicBlock *> inWorklist;
};
}

#endif