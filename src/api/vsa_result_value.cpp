#include <llvm/Analysis/LazyValueInfo.h>

#include "vsa_result_value.h"
using namespace llvm;

namespace pcpo {

    LazyValueInfo::Tristate VsaResultValue::testIf(CmpInst::Predicate predicate, Constant* C) {

        if (ConstantInt::classof(C)) {
            auto temp = AD_TYPE(reinterpret_cast<ConstantInt *> (C)->getValue());
            auto result = abstractValue->icmp(predicate, C->getType()->getIntegerBitWidth(), temp);

            bool b_t = !result.first->isBottom();
            bool b_f = !result.second->isBottom();

            if (b_t && b_f)
                return LazyValueInfo::Unknown;
            if (b_f)
                return LazyValueInfo::False;
            else
                return LazyValueInfo::True;
        } else {
            return LazyValueInfo::Unknown;
        }
        
    }

}
