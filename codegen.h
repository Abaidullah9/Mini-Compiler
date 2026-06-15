#ifndef CODEGEN_H
#define CODEGEN_H

#include "tac.h"

class CodeGenerator {
public:
    static void generateAssembly(const vector<TAC>& tacList);
};

#endif
