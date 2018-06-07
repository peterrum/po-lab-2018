#include "worklist.h"
#include "state.h"

using namespace llvm;
namespace pcpo {
    
    void WorkList::push(BasicBlock* bb){
        if(inWorklist.find(bb)==inWorklist.end()){
            worklist.push(bb);
            inWorklist.insert(bb);
        }
    }
    
    BasicBlock* WorkList::pop(){
        auto temp = worklist.front();
        inWorklist.erase(temp);
        worklist.pop();
        return temp;
    }
    
    bool WorkList::empty(){
        return worklist.empty();
    }
    
}