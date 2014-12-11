//===----------------------------- SAGEExpr.cpp ---------------------------===//
//===----------------------------------------------------------------------===//

#include "SAGEExpr.h"

#include "llvm/IR/Argument.h"
#include "llvm/Support/raw_ostream.h"

#include <map>

using namespace llvm;

raw_ostream& operator<<(raw_ostream& OS, const SAGEExpr &E) {
  OS << *E.getExpr();
  return OS;
}

SAGEExpr::SAGEExpr(SAGEInterface &SI)
  : SI_(SI), Expr_((PyObject*)1) {
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, int Int)
  : SI_(SI), Expr_(SI.var(Int)) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, APInt Int)
  : SI_(SI), Expr_(SI.var(Int.getSExtValue())) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, Twine Name)
  : SI_(SI), Expr_(SI.var(Name.str().c_str())) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, PyObject *Obj)
  : SI_(SI), Expr_(Obj) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, const Value *V)
  : SI_(SI) {
  assert(V && "Constructor expected non-null parameter");
  static std::map<const Value*, PyObject*> SAGEExprs;

  if (const ConstantInt *CI = dyn_cast<ConstantInt>(V)) {
    Expr_ = SI_.var(CI->getValue().getSExtValue());
    assert(Expr_);
    return;
  }

  if (SAGEExprs.count(V)) {
    Expr_ = SAGEExprs[V];
    assert(Expr_);
    return;
  }

  Twine Name;
  if (V->hasName()) {
    if (isa<const Instruction>(V) || isa<const Argument>(V))
      Name = "_" + V->getName();
    else
      Name = "__SRA_SYM_UNKNOWN_" + V->getName() + "__";
  } else {
    Name = "__SRA_SYM_UNAMED__";
  }

  Expr_ = SI_.var(Name.str().c_str());
  assert(Expr_);
  SAGEExprs[V] = Expr_;
}

SAGEExpr SAGEExpr::operator+(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.add(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator+(long Other) const {
  return SAGEExpr(SI_, SI_.add(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator-(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.sub(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator-(long Other) const {
  return SAGEExpr(SI_, SI_.sub(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator*(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.mul(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator*(long Other) const {
  return SAGEExpr(SI_, SI_.mul(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator/(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.div(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator/(long Other) const {
  return SAGEExpr(SI_, SI_.div(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator^(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.pow(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator^(long Other) const {
  return SAGEExpr(SI_, SI_.pow(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator==(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.eq(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator==(long Other) const {
  return SAGEExpr(SI_, SI_.eq(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator!=(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.ne(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator!=(long Other) const {
  return SAGEExpr(SI_, SI_.ne(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator<(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.lt(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator<(long Other) const {
  return SAGEExpr(SI_, SI_.lt(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator<=(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.le(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator<=(long Other) const {
  return SAGEExpr(SI_, SI_.le(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator>(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.gt(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator>(long Other) const {
  return SAGEExpr(SI_, SI_.gt(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator>=(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.ge(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator>=(long Other) const {
  return SAGEExpr(SI_, SI_.ge(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::operator-() const {
  return SAGEExpr(SI_, SI_.neg(Expr_));
}

SAGEExpr SAGEExpr::operator&&(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_._and(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator||(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_._or(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::operator!() const {
  return SAGEExpr(SI_, SI_._not(Expr_));
}

SAGEExpr SAGEExpr::min(const SAGEExpr& Other,
                       const SAGEExpr& Assumptions) const {
  return SAGEExpr(SI_, SI_.min(Expr_, Other.getExpr(), Assumptions.getExpr()));
}

SAGEExpr SAGEExpr::min(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.min(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::min(const long Other) const {
  return SAGEExpr(SI_, SI_.min(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::max(const SAGEExpr& Other,
                       const SAGEExpr& Assumptions) const {
  return SAGEExpr(SI_, SI_.max(Expr_, Other.getExpr(), Assumptions.getExpr()));
}

SAGEExpr SAGEExpr::max(const SAGEExpr& Other) const {
  return SAGEExpr(SI_, SI_.max(Expr_, Other.getExpr()));
}

SAGEExpr SAGEExpr::max(const long Other) const {
  return SAGEExpr(SI_, SI_.max(Expr_, SI_.var(Other)));
}

SAGEExpr SAGEExpr::negation() const {
  return SAGEExpr(SI_, SI_.neg(Expr_));
}

void SAGEExpr::operator=(const SAGEExpr& Other) {
  Expr_ = Other.getExpr();
  assert(Expr_);
}

void SAGEExpr::operator=(SAGEExpr& Other) {
  Expr_ = Other.getExpr();
  assert(Expr_);
}

bool SAGEExpr::isEQ(const SAGEExpr &Other) const {
  return SI_.isEQ(Expr_, Other.getExpr());
}

bool SAGEExpr::isNE(const SAGEExpr &Other) const {
  return SI_.isNE(Expr_, Other.getExpr());
}

bool SAGEExpr::isConstant() const {
  return SI_.isConstant(Expr_);
}

SAGEExpr SAGEExpr::getNaN(SAGEInterface &SI) {
  static SAGEExpr NaN(SI, SI.getNaN());
  return NaN;
}

SAGEExpr SAGEExpr::getPlusInf(SAGEInterface &SI) {
  static SAGEExpr Inf(SI, SI.getPlusInf());
  return Inf;
}

SAGEExpr SAGEExpr::getMinusInf(SAGEInterface &SI) {
  static SAGEExpr Inf(SI, SI.getMinusInf());
  return Inf;
}

SAGEExpr SAGEExpr::getTrue(SAGEInterface &SI) {
  static SAGEExpr True(SI, SI.getTrue());
  return True;
}

SAGEExpr SAGEExpr::getFalse(SAGEInterface &SI) {
  static SAGEExpr False(SI, SI.getFalse());
  return False;
}

int SAGEExpr::compare(const SAGEExpr& Other) const {
  return SI_.compare(Expr_, Other.getExpr());
}

PyObject *SAGEExpr::getExpr() const {
  return Expr_;
}

