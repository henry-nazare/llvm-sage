//===------------------------ SAGEInterfaceMILP.cpp -----------------------===//
//===----------------------------------------------------------------------===//

#include "SAGEInterfaceMILP.h"

#include "llvm/Support/CommandLine.h"

using namespace llvm;

static RegisterPass<SAGEInterfaceMILP>
  X("sage-interface-milp", "SAGE/LLVM MILP interface");
char SAGEInterfaceMILP::ID = 0;

void SAGEInterfaceMILP::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<PythonInterface>();
  AU.setPreservesAll();
}

bool SAGEInterfaceMILP::runOnModule(Module& M) {
  PI_ = &getAnalysis<PythonInterface>();

  ObjVec_ = PI_->createObjVec(getObjVecInit());
  assert(ObjVec_);

  return false;
}

PyObject *SAGEInterfaceMILP::createMILP() {
  return PI_->call(ObjVec_, FN_MILP_INIT, {});
}

PyObject *SAGEInterfaceMILP::addConstraint(PyObject *MILP, PyObject *Expr) {
  return PI_->callSelf("add_constraint", MILP, {Expr});
}

PyObject *SAGEInterfaceMILP::setObjective(PyObject *MILP, PyObject *Expr) {
  return PI_->callSelf("set_objective", MILP, {Expr});
}

PyObject *SAGEInterfaceMILP::solve(PyObject *MILP) {
  return PI_->callSelf("solve", MILP, {});
}

