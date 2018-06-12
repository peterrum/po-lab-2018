#include "state.h"
#include "util.h"
#include <stdlib.h>
using namespace llvm;

namespace pcpo {
  class VsaResultValue {

  public:
    VsaResultValue(std::shared_ptr<AbstractDomain> abstractValue) : abstractValue(abstractValue) {}

  private:
    std::shared_ptr<AbstractDomain> abstractValue;
  };
}
