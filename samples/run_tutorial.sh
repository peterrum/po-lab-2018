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

# color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# if no folder is existent
mkdir -p build

# for all c-files...
for f in ${ARRAY[*]};
do
    # ... print file name
    echo "###############################################################################"
    echo $(pwd)/$f
    # ... clean up for old run
    rm -f build/$f.out
    rm -f build/$f.bc
    rm -f build/$f-opt.bc
    # ... compile
    $VSA_CLANG_PATH/bin/clang -O0 -emit-llvm $f -Xclang -disable-O0-optnone -c -o build/$f.bc
    # ... run mem2reg optimization
    $VSA_LLVM_PATH/bin/opt -mem2reg < build/$f.bc > build/$f-opt.bc
    # ... disassemble optimized file
    $VSA_LLVM_PATH/bin/llvm-dis build/$f-opt.bc
    # ... run VSA
    $VSA_LLVM_PATH/bin/opt -load $VSA_LLVM_PATH/lib/llvm-vsa-tutorial.so -vsatutorialpass < build/$f-opt.bc > /dev/null 2> >(tee build/$f.out >&2)
    cp -n build/$f.out build/$f.ref
done

printf "\nTEST SUMMARY:\n"
for f in ${ARRAY[*]};
do
    if cmp build/$f.out build/$f.ref; then
        # ... success
        printf "Run: ${GREEN}$f${NC}\n"
    else
        # ... failure
        printf "Run: ${RED}$f${NC}\n"
    fi
done
printf "\n"
