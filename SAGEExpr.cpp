//===----------------------------- SAGEExpr.cpp ---------------------------===//
//===----------------------------------------------------------------------===//

#include "SAGEExpr.h"

using namespace llvm;
using llvmpy::Get;

struct ExprObjInfo : public PythonObjInfo {
  ExprObjInfo(const char *Fn) : PythonObjInfo("llvmsage.expr", "Expr", Fn) {}
};

SAGEExpr::SAGEExpr(PyObject *Obj) : llvmpy::PyObjectHolder(Obj) {
}

static ExprObjInfo expr_Expr(nullptr);
SAGEExpr::SAGEExpr(int Int) : SAGEExpr(expr_Expr({Get(Int)})) {
}

SAGEExpr SAGEExpr::operator*(const SAGEExpr &Other) const {
  static ExprObjInfo expr_Expr___mul__("__mul__");
  return expr_Expr___mul__({get(), Other.get()});
}

SAGEExpr SAGEExpr::GetMinusInf() {
  static ExprObjInfo expr_Expr_get_minus_inf("get_minus_inf");
  return expr_Expr_get_minus_inf({});
}

