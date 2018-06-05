# Program Optimization Lab

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