#ifndef VSA_RESULT_H_
#define VSA_RESULT_H_

#include "../fixpoint/state.h"
#include "../util/util.h"
#include "vsa_result_value.h"
#include "llvm/IR/BasicBlock.h"
#include <memory>

using namespace llvm;

namespace pcpo {
class VsaResult {

public:
  VsaResult(std::map<BasicBlock *, State> &globalProgramPoints)
      : globalProgramPoints(globalProgramPoints) {}

  void print() const;

  bool isReachable(BasicBlock *BB) const;

  bool isResultAvailable(BasicBlock *BB, Value *val) const;

  std::unique_ptr<VsaResultValue> getAbstractValue(BasicBlock *BB, Value *val) const;

private:
  std::map<BasicBlock *, State> &globalProgramPoints;
};
}

#endif