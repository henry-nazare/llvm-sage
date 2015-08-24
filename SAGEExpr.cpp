//===----------------------------- SAGEExpr.cpp ---------------------------===//
//===----------------------------------------------------------------------===//

#include "SAGEExpr.h"

using namespace llvm;

SAGEExpr::SAGEExpr(PyObject *Obj) : llvmpy::PyObjectHolder(Obj) {
}

