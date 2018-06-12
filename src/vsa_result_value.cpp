#include <llvm/Analysis/LazyValueInfo.h>

#include "vsa_result_value.h"
using namespace llvm;

namespace pcpo {

    LazyValueInfo::Tristate VsaResultValue::testIf(CmpInst::Predicate /*predicate*/, Constant* /*C*/){
        return LazyValueInfo::Unknown;
    }
    
}
