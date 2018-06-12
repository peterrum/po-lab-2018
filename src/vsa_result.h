#ifndef VSA_RESULT_H_
#define VSA_RESULT_H_

#include "state.h"
#include "llvm/IR/BasicBlock.h"
#include "util.h"
#include "vsa_result_value.h"
#include <memory>

using namespace llvm;

namespace pcpo {
  class VsaResult {

  public:
  VsaResult(std::map<BasicBlock*, State>& globalProgramPoints) : globalProgramPoints(globalProgramPoints) {}

  void print();

  bool isReachable(BasicBlock* BB);

  bool isResultAvailable(BasicBlock* BB, Value* val);

  std::unique_ptr<VsaResultValue> getAbstractValue(BasicBlock* BB, Value* val);

  private:
    std::map<BasicBlock*, State>& globalProgramPoints;
  };
}

#endif