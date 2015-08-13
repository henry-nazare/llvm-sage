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
  return PI_->call(ObjVec_, FN_EXPR_INIT, {PyLong_FromLong(Val)});
}

PyObject *SAGEInterface::var(const unsigned long Val) {
  return PI_->call(ObjVec_, FN_EXPR_INIT, {PyLong_FromUnsignedLong(Val)});
}

PyObject *SAGEInterface::var(const char *Str) {
  return PI_->call(ObjVec_, FN_EXPR_INIT, {PyString_FromString(Str)});
}

std::string SAGEInterface::getName(PyObject *Expr) {
  return PythonInterface::toString(PI_->callSelf("get_name", Expr, {}));
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

std::vector<PyObject*> SAGEInterface::args(PyObject *Expr) {
  return PythonInterface::toVector(PI_->callSelf("args", Expr, {}));
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

long SAGEInterface::getInteger(PyObject *Expr) {
  auto Obj = PI_->callSelf("get_integer", Expr, {});
  errs() << "Obj: " << Obj << "\n";
  return PyInt_AsLong(Obj);
}

PyObject *SAGEInterface::getNumer(PyObject *Expr) {
  return PI_->callSelf("get_numer", Expr, {});
}

PyObject *SAGEInterface::getDenom(PyObject *Expr) {
  return PI_->callSelf("get_denom", Expr, {});
}

PyObject *SAGEInterface::getBase(PyObject *Expr) {
  return PI_->callSelf("get_base", Expr, {});
}

PyObject *SAGEInterface::getExp(PyObject *Expr) {
  return PI_->callSelf("get_exp", Expr, {});
}

bool SAGEInterface::isEQ(PyObject *First, PyObject *Second) {
  return PI_->callSelf("is_eq", First, {Second}) == Py_True;
}

bool SAGEInterface::isNE(PyObject *First, PyObject *Second) {
  return PI_->callSelf("is_ne", First, {Second}) == Py_True;
}

bool SAGEInterface::isConstant(PyObject *Expr) {
  return PI_->callSelf("is_constant", Expr, {}) == Py_True;
}

bool SAGEInterface::isInteger(PyObject *Expr) {
  return PI_->callSelf("is_integer", Expr, {}) == Py_True;
}

bool SAGEInterface::isRational(PyObject *Expr) {
  return PI_->callSelf("is_rational", Expr, {}) == Py_True;
}

bool SAGEInterface::isSymbol(PyObject *Expr) {
  return PI_->callSelf("is_symbol", Expr, {}) == Py_True;
}

bool SAGEInterface::isMin(PyObject *Expr) {
  return PI_->callSelf("is_min", Expr, {}) == Py_True;
}

bool SAGEInterface::isMax(PyObject *Expr) {
  return PI_->callSelf("is_max", Expr, {}) == Py_True;
}

bool SAGEInterface::isAdd(PyObject *Expr) {
  return PI_->callSelf("is_add", Expr, {}) == Py_True;
}

bool SAGEInterface::isMul(PyObject *Expr) {
  return PI_->callSelf("is_mul", Expr, {}) == Py_True;
}

bool SAGEInterface::isPow(PyObject *Expr) {
  return PI_->callSelf("is_pow", Expr, {}) == Py_True;
}

bool SAGEInterface::isInf(PyObject *Expr) {
  return PI_->callSelf("is_inf", Expr, {}) == Py_True;
}

bool SAGEInterface::isPlusInf(PyObject *Expr) {
  return PI_->callSelf("is_plus_inf", Expr, {}) == Py_True;
}

bool SAGEInterface::isMinusInf(PyObject *Expr) {
  return PI_->callSelf("is_minus_inf", Expr, {}) == Py_True;
}

int SAGEInterface::compare(PyObject *First, PyObject *Second) {
  PyObject *Res = PI_->callSelf("compare", First, {Second});
  assert(PyInt_Check(Res) && "Result should be an integer");
  return (int)PyInt_AsLong(Res);
}

