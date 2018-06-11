#!/bin/bash

# Script to run all VSA on all test c-programs.
# Please specify the path to llvm and clang in the environment variables
# VSA_CLANG_PATH and VSA_LLVM_PATH.

# if one argument passed: only analyze the passed program
if [ $# == 1 ] ; then
    ARRAY=($1) 
else # run all
    ARRAY=($(ls -d *.c))
fi

# for all c-files...
for f in ${ARRAY[*]};
do
    # ... print file name
    echo "###############################################################################"
    echo pwd/$f
    # ... compile
    $VSA_CLANG_PATH/bin/clang -O0 -emit-llvm $f -Xclang -disable-O0-optnone -c -o build/$f.bc
    # ... run mem2reg optimization
    $VSA_LLVM_PATH/bin/opt -mem2reg < build/$f.bc > build/$f-opt.bc
    # ... disassemble optimized file
    $VSA_LLVM_PATH/bin/llvm-dis build/$f-opt.bc
    # ... run VSA
    $VSA_LLVM_PATH/bin/opt -load $VSA_LLVM_PATH/lib/llvm-vsa.so -vsapass < build/$f-opt.bc > /dev/null
done