#include "vsa-benchmark.h"

namespace {

bool VsaBenchmarkPass::runOnModule(Module &M) {
    uint64_t vsa_count = 0;
    uint64_t lvi_count = 0;
    uint64_t eq_count = 0;

    for (auto &f : M.functions()) {
        /// check if function is empty
        if(f.begin()==f.end())
            continue;

        /// extract results from VsaPass
        const auto& vsa = getAnalysis<VsaPass>(f).getResult();

        /// get LazyValueInfo for function
        LazyValueInfo& lvi = getAnalysis<LazyValueInfoWrapperPass>(f).getLVI();

        for (auto& bb : f) {
            /// block not reachable in vsa -> vsa.value = bottom
            if (!vsa.isReachable(&bb)){
                for (auto &instr: bb) {
                    if(!instr.getType()->isIntegerTy()) continue;
                    /// extract results of variable i
                    const llvm::ConstantRange lvi_const_range = lvi.getConstantRange(&instr, &bb);
                    /// both know it is bottom
                    if(lvi_const_range.isEmptySet()) eq_count++;
                    /// lvi thinks it's not bottom
                    else vsa_count++;
                }
                continue;
            }

            for (auto &instr: bb) {
                if(!instr.getType()->isIntegerTy()) continue;
                const llvm::ConstantRange lvi_const_range = lvi.getConstantRange(&instr, &bb);

                /// no results in vsa -> vsa.value = top
                if(!vsa.isResultAvailable(&bb, &instr)) {
                    /// both T
                    if (lvi_const_range.isFullSet()) eq_count++;
                    /// lvi has something better
                    else lvi_count++;
                    continue;
                }

                const auto abstractValue = vsa.getAbstractValue(&bb, &instr);
                if(abstractValue->isTop()){
                    /// both T
                    if(lvi_const_range.isFullSet()) eq_count++;
                    /// lvi has something better
                    else lvi_count++;
                    continue;
                }

                /// Comparing set sizes
                const auto vsa_size = abstractValue->getNumValues().getZExtValue();
                const auto lvi_size = lvi_const_range.getSetSize().getZExtValue();

                /// both same size
                if(vsa_size == lvi_size) eq_count++;
                /// vsa smaller -> better
                else if(vsa_size < lvi_size) vsa_count++;
                /// lvi smaller -> better
                else lvi_count++;
            }
        }
    }

    errs() << vsa_count << "," << lvi_count << "," << eq_count << '\n';

    /// analysis has made no modifications
    return false;
}
} //namespace
