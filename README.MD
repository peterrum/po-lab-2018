# Program Optimization Lab 2018

Aim of the [Program Optimization Lab 2018](http://www2.in.tum.de/hp/Main?nid=366) 
is to explore the LLVM opt tool and extend it with an advanced Value-Set Analysis
according to [Reps et al.](http://research.cs.wisc.edu/wpis/papers/pepm06.invited.pdf)

## Authors (in alphabetic order)

* TODO

## Installation

Clone the project into the following folder:
```bash
cd llvm/lib/Transforms
git clone git@github.com:peterrum/po-lab-2018.git
mv po-lab-2018 ValueSetAnalysis
```

and add the following line to the `CMakeLists.txt`-file:
```bash
add_subdirectory(ValueSetAnalysis)
```

Finally run `make` and/or `make install` again.

## How to ...

### ... view IR

Recompile and view output with your favorite text editor: 
```bash
clang -Os -S -emit-llvm hello.c -o hello.ll
```

### ... use opt

```bash
clang -O3 -emit-llvm hello.c -c -o hello.bc
opt -load llvm/lib/llvm-vsa.so -hello < hello.bc > /dev/null
```

## Useful methods in LLVM

* Write output: errs() << "Hello World!\n"

### llvm::Module
* Iterator over all functions

### llvm::Function
* Iterator over all basic blocks
* getName()

### llvm::BasicBlock
* Iterator over all instructions

### llvm::Instruction
* getOpcodeName()
* getValueID()
* users()

### llvm::InstVisitor<U, V>
* V visit(...)
