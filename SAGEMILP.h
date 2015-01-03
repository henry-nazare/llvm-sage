#ifndef _SAGEMILP_H_
#define _SAGEMILP_H_

#include <python2.7/Python.h>

#include "SAGEExpr.h"
#include "SAGEInterfaceMILP.h"

#include <map>

using namespace llvm;

class SAGEMILP {
public:
  SAGEMILP(SAGEInterfaceMILP &SIM)
    : SIM_(SIM), MILP_(SIM.createMILP()) {
  }

  void addConstraint(const SAGEExpr &Expr);
  void setObjective(const SAGEExpr &Expr);

  std::map<std::string, long> solve();

private:
  SAGEInterfaceMILP &SIM_;
  PyObject *MILP_;
};

#endif

