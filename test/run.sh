echo "The test runner relies on the presence of build/add-1.c.bc as a dummy input"
echo "Run ../samples/run.sh to create this file"
echo "--------------- BEGIN TESTS ------------------------"

$VSA_LLVM_PATH/bin/opt -load $VSA_LLVM_PATH/lib/llvm-vsa.so -testBS < ../samples/build/add-1.c.bc > /dev/null
