//===----------------------------- SAGEExpr.cpp ---------------------------===//
//===----------------------------------------------------------------------===//

#include "SAGEExpr.h"

#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#include <map>
#include <string>

using namespace llvm;

raw_ostream& operator<<(raw_ostream& OS, const SAGEExpr &E) {
  OS << *E.getExpr();
  return OS;
}

SAGEExpr::SAGEExpr(SAGEInterface &SI)
  : SI_(SI), Expr_((PyObject*)1) {
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, int Int)
  : SI_(SI), Expr_(SI.var((long) Int)) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, uint64_t Int)
  : SI_(SI), Expr_(SI.var(Int)) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, int64_t Int)
  : SI_(SI), Expr_(SI.var(Int)) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, const Twine &Name)
  : SI_(SI), Expr_(SI.var(Name.str().c_str())) {
  assert(Expr_ != nullptr);
}

SAGEExpr::SAGEExpr(SAGEInterface &SI, PyObject *Obj)
  : SI_(SI), Expr_(Obj) {
  assert(Expr_ != nullptr);
}

std::string SAGEExpr::getName() const {
  return SI_.getName(Expr_);
}

long SAGEExpr::getInteger() const {
  return SI_.getInteger(Expr_);
}

long SAGEExpr::getSize() const {
  return SI_.getSize(Expr_);
}

SAGEExpr SAGEExpr::getNumer() const {
  return SAGEExpr(SI_, SI_.getNumer(Expr_));
}

SAGEExpr SAGEExpr::getDenom() const {
  return SAGEExpr(SI_, SI_.getDenom(Expr_));
}

SAGEExpr SAGEExpr::getExp() const {
  return SAGEExpr(SI_, SI_.getExp(Expr_));
}

SAGEExpr SAGEExpr::getBase() const {
  return SAGEExpr(SI_, SI_.getBase(Expr_));
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
  if (isMinusInf()) {
    return Other.isPlusInf() ? getNaN(SI_) : *this;
  } else if (isPlusInf()) {
    return Other.isMinusInf() ? getNaN(SI_) : *this;
  } else if (Other.isMinusInf() || Other.isPlusInf()) {
    return Other;
  }

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

std::vector<SAGEExpr> SAGEExpr::args() const {
  std::vector<PyObject*> Objs = SI_.args(Expr_);
  std::vector<SAGEExpr> Ret;
  for (auto Obj : Objs)
    Ret.push_back(SAGEExpr(SI_, Obj));
  return Ret;
}

Value *SAGEExpr::toValue(IntegerType *Ty, IRBuilder<> &IRB,
    const std::map<std::string, Value*> &Values, Module *M) const {
  LLVMContext &Context = M->getContext();
  // +oo, -oo.
  if (isMinusInf()) {
    APInt Int = APInt::getSignedMinValue(Ty->getBitWidth());
    return Constant::getIntegerValue(Ty, Int);
  } else  if (isPlusInf()) {
    APInt Int = APInt::getSignedMaxValue(Ty->getBitWidth());
    return Constant::getIntegerValue(Ty, Int);
  }

  // Integers, symbols.
  if (isInteger()) {
    APInt Int(Ty->getBitWidth(), getInteger(), true);
    return Constant::getIntegerValue(Ty, Int);
  } else if (isRational()) {
    Value *Numer = getNumer().toValue(Ty, IRB, Values, M);
    Value *Denom = getDenom().toValue(Ty, IRB, Values, M);
    return IRB.CreateSDiv(Numer, Denom);
  } else if (isSymbol()) {
    auto It = Values.find(getName());
    assert(It != Values.end() && "Value not contained in map");
    Value *V = It->second;

    if (V->getType() == Ty)
      return V;
    else if (V->getType()->getIntegerBitWidth() < Ty->getBitWidth())
      return IRB.CreateSExt(V, Ty);
    else
      return IRB.CreateTrunc(V, Ty);
  }

  // Exponentiation, addition, mulitplication.
  if (isPow()) {
    std::vector<SAGEExpr> Args = args();
    Value *Base = Args.at(0).toValue(Ty, IRB, Values, M);
    Value *Exp  = Args.at(1).toValue(Ty, IRB, Values, M);

    Type  *DoubleTy   = Type::getDoubleTy(Context);
    Value *BaseDouble = IRB.CreateSIToFP(Base, DoubleTy);
    Value *ExpDouble  = IRB.CreateSIToFP(Exp,  DoubleTy);

    Value *PowFn = Intrinsic::getDeclaration(M, Intrinsic::pow,
                                                ArrayRef<Type*>(DoubleTy));
    Value *Pow =
        IRB.CreateCall(PowFn, ArrayRef<Value*>({BaseDouble, ExpDouble}));
    Value *Cast = IRB.CreateFPToSI(Pow, Base->getType());
    return Cast;
  } else if (isAdd() || isMul()) {
    bool IsAdd = isAdd();
    // The accumulator will keep track of the last generated expression, to be
    // used in the next iteration.
    Value *Acc = nullptr;
    for (auto Expr : args()) {
      // TODO: Divisions might not be handled correctly here.
      Value *Curr = Expr.toValue(Ty, IRB, Values, M);
      Acc = Acc ? IsAdd ? IRB.CreateAdd(Acc, Curr) : IRB.CreateMul(Acc, Curr)
                : Curr;
    }
    return Acc;
  }

  // Min, max.
  if (isMin()) {
    Value *Min = nullptr;
    for (auto Expr : args()) {
      Value *Curr = Expr.toValue(Ty, IRB, Values, M);
      if (Min) {
        Value *Cmp = IRB.CreateICmp(CmpInst::ICMP_SLT, Min, Curr);
        Min = IRB.CreateSelect(Cmp, Min, Curr);
      } else {
        Min = Curr;
      }
    }
    return Min;
  } else if (isMax()) {
    Value *Max = nullptr;
    for (auto Expr : args()) {
      Value *Curr = Expr.toValue(Ty, IRB, Values, M);
      if (Max) {
        Value *Cmp = IRB.CreateICmp(CmpInst::ICMP_SGT, Max, Curr);
        Max = IRB.CreateSelect(Cmp, Max, Curr);
      } else {
        Max = Curr;
      }
    }
    return Max;
  }

  assert(false && "Unhandled expression");
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

bool SAGEExpr::isInteger() const {
  return SI_.isConstant(Expr_);
}

bool SAGEExpr::isRational() const {
  return SI_.isRational(Expr_);
}

bool SAGEExpr::isSymbol() const {
  return SI_.isSymbol(Expr_);
}

bool SAGEExpr::isMin() const {
  return SI_.isMin(Expr_);
}

bool SAGEExpr::isMax() const {
  return SI_.isMax(Expr_);
}

bool SAGEExpr::isAdd() const {
  return SI_.isAdd(Expr_);
}

bool SAGEExpr::isMul() const {
  return SI_.isMul(Expr_);
}

bool SAGEExpr::isPow() const {
  return SI_.isPow(Expr_);
}

bool SAGEExpr::isInf() const {
  return SI_.isInf(Expr_);
}

bool SAGEExpr::isPlusInf() const {
  return SI_.isPlusInf(Expr_);
}

bool SAGEExpr::isMinusInf() const {
  return SI_.isMinusInf(Expr_);
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

