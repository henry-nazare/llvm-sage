#ifndef _SAGEEXPR_H_
#define _SAGEEXPR_H_

#include "Python/PythonInterface.h"

using namespace llvm;

class SAGEExpr : public llvmpy::PyObjectHolder {
public:
  SAGEExpr(PyObject *Obj);
  SAGEExpr(int Int);

  SAGEExpr operator*(const SAGEExpr &Other) const;

  static SAGEExpr GetMinusInf();
};

#endif

