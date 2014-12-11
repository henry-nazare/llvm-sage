//===-------------------------- SAGEInterface.cpp -------------------------===//
//===----------------------------------------------------------------------===//

#include "SAGEInterface.h"

#include "llvm/Support/CommandLine.h"

using namespace llvm;

static RegisterPass<SAGEInterface> X("sage-interface", "SAGE/LLVM interface");
char SAGEInterface::ID = 0;

void SAGEInterface::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<PythonInterface>();
  AU.setPreservesAll();
}

bool SAGEInterface::runOnModule(Module& M) {
  PI_ = &getAnalysis<PythonInterface>();

  ObjVec_ = PI_->createObjVec(getObjVecInit());
  assert(ObjVec_);

  return false;
}

PyObject *SAGEInterface::var(const long Val) {
  return PI_->call(ObjVec_, FN_EXPR_INIT, {PyInt_FromLong(Val)});
}

PyObject *SAGEInterface::var(const char *Str) {
  return PI_->call(ObjVec_, FN_EXPR_INIT, {PyString_FromString(Str)});
}

PyObject *SAGEInterface::add(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__add__", LHS, {RHS});
}

PyObject *SAGEInterface::sub(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__sub__", LHS, {RHS});
}

PyObject *SAGEInterface::mul(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__mul__", LHS, {RHS});
}

PyObject *SAGEInterface::div(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__div__", LHS, {RHS});
}

PyObject *SAGEInterface::pow(PyObject *Base, PyObject *Exp) {
  return PI_->callSelf("__pow__", Base, {Exp});
}

PyObject *SAGEInterface::neg(PyObject *Expr) {
  return PI_->callSelf("__neg__", Expr, {});
}

PyObject *SAGEInterface::eq(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__eq__", LHS, {RHS});
}

PyObject *SAGEInterface::ne(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__ne__", LHS, {RHS});
}

PyObject *SAGEInterface::lt(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__lt__", LHS, {RHS});
}

PyObject *SAGEInterface::le(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__le__", LHS, {RHS});
}

PyObject *SAGEInterface::gt(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__gt__", LHS, {RHS});
}

PyObject *SAGEInterface::ge(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__ge__", LHS, {RHS});
}

PyObject *SAGEInterface::_and(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__and__", LHS, {RHS});
}

PyObject *SAGEInterface::_or(PyObject *LHS, PyObject *RHS) {
  return PI_->callSelf("__or__", LHS, {RHS});
}

PyObject *SAGEInterface::_not(PyObject *Expr) {
  return PI_->callSelf("__invert__", Expr, {});
}

PyObject *SAGEInterface::min(PyObject *First, PyObject *Second) {
  return PI_->callSelf("min", First, {Second});
}

PyObject *SAGEInterface::min(PyObject *First, PyObject *Second,
                             PyObject *Assumptions) {
  return PI_->callSelf("min", First, {Second, Assumptions});
}

PyObject *SAGEInterface::max(PyObject *First, PyObject *Second) {
  return PI_->callSelf("max", First, {Second});
}

PyObject *SAGEInterface::max(PyObject *First, PyObject *Second,
                             PyObject *Assumptions) {
  return PI_->callSelf("max", First, {Second, Assumptions});
}

PyObject *SAGEInterface::getNaN() {
  return PI_->call(ObjVec_, FN_NAN, {});
}

PyObject *SAGEInterface::getPlusInf() {
  return PI_->call(ObjVec_, FN_PLUS_INF, {});
}

PyObject *SAGEInterface::getMinusInf() {
  return PI_->call(ObjVec_, FN_MINUS_INF, {});
}

PyObject *SAGEInterface::getTrue() {
  return PI_->call(ObjVec_, FN_TRUE, {});
}

PyObject *SAGEInterface::getFalse() {
  return PI_->call(ObjVec_, FN_FALSE, {});
}

bool SAGEInterface::isEQ(PyObject *First, PyObject *Second) {
  return PI_->callSelf("is_eq", First, {Second}) == Py_True;
}

bool SAGEInterface::isNE(PyObject *First, PyObject *Second) {
  return PI_->callSelf("is_ne", First, {Second}) == Py_True;
}

int SAGEInterface::compare(PyObject *First, PyObject *Second) {
  PyObject *Res = PI_->callSelf("compare", First, {Second});
  assert(PyInt_Check(Res) && "Result should be an integer");
  return (int)PyInt_AsLong(Res);
}

