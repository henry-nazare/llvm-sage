#include "SAGEMILP.h"

void SAGEMILP::addConstraint(const SAGEExpr &Expr) {
  SIM_.addConstraint(MILP_, Expr.getExpr());
}

void SAGEMILP::setObjective(const SAGEExpr &Expr) {
  SIM_.setObjective(MILP_, Expr.getExpr());
}

std::map<std::string, long> SAGEMILP::solve() {
  PyObject *Solution = SIM_.solve(MILP_), *Key, *Val;
  Py_ssize_t Pos = 0;
  std::map<std::string, long> Ret;
  while (PyDict_Next(Solution, &Pos, &Key, &Val))
    Ret.insert(std::make_pair(PyString_AsString(Key), PyInt_AsLong(Val)));
  return Ret;
}

