#ifndef _SAGEEXPR_H_
#define _SAGEEXPR_H_

#include "SAGEInterface.h"

#include "llvm/IR/IRBuilder.h"

using namespace llvm;

class SAGEExpr {
public:
  SAGEExpr(SAGEInterface &SI);
  SAGEExpr(SAGEInterface &SI, int Int);
  SAGEExpr(SAGEInterface &SI, APInt Int);
  SAGEExpr(SAGEInterface &SI, const Twine &Name);
  SAGEExpr(SAGEInterface &SI, PyObject *Obj);
  SAGEExpr(SAGEInterface &SI, const Value *V);

  PyObject *getExpr()   const;

  std::string getName() const;
  long getInteger()     const;

  SAGEExpr operator+(const SAGEExpr& Other)  const;
  SAGEExpr operator+(long Other)             const;
  SAGEExpr operator-(const SAGEExpr& Other)  const;
  SAGEExpr operator-(long Other)             const;
  SAGEExpr operator*(const SAGEExpr& Other)  const;
  SAGEExpr operator*(long Other)             const;
  SAGEExpr operator/(const SAGEExpr& Other)  const;
  SAGEExpr operator/(long Other)             const;
  SAGEExpr operator^(const SAGEExpr& Other)  const;
  SAGEExpr operator^(long Other)             const;
  SAGEExpr operator==(const SAGEExpr& Other) const;
  SAGEExpr operator==(long Other)            const;
  SAGEExpr operator!=(const SAGEExpr& Other) const;
  SAGEExpr operator!=(long Other)            const;
  SAGEExpr operator<(const SAGEExpr& Other)  const;
  SAGEExpr operator<(long Other)             const;
  SAGEExpr operator<=(const SAGEExpr& Other) const;
  SAGEExpr operator<=(long Other)            const;
  SAGEExpr operator>(const SAGEExpr& Other)  const;
  SAGEExpr operator>(long Other)             const;
  SAGEExpr operator>=(const SAGEExpr& Other) const;
  SAGEExpr operator>=(long Other)            const;

  SAGEExpr operator-() const;

  SAGEExpr operator&&(const SAGEExpr& Other) const;
  SAGEExpr operator||(const SAGEExpr& Other) const;
  SAGEExpr operator!()                       const;

  SAGEExpr min(const SAGEExpr& Other, const SAGEExpr& Assumptions) const;
  SAGEExpr min(const SAGEExpr& Other)                              const;
  SAGEExpr min(long Other)                                         const;
  SAGEExpr max(const SAGEExpr& Other, const SAGEExpr& Assumptions) const;
  SAGEExpr max(const SAGEExpr& Other)                              const;
  SAGEExpr max(long Other)                                         const;

  SAGEExpr negation() const;

  std::vector<SAGEExpr> args() const;

  Value *toValue(IntegerType *Ty, IRBuilder<> &IRB,
                 std::map<std::string, Value*> Value, Module *M) const;

  void operator=(SAGEExpr& Other);
  void operator=(const SAGEExpr& Other);

  bool isEQ(const SAGEExpr &Other) const;
  bool isNE(const SAGEExpr &Other) const;

  bool isConstant() const;
  bool isInteger() const;
  bool isRational() const;
  bool isSymbol() const;

  bool isMin() const;
  bool isMax() const;

  bool isAdd() const;
  bool isMul() const;
  bool isPow() const;

  bool isInf() const;
  bool isPlusInf() const;
  bool isMinusInf() const;

  int compare(PyObject *First, PyObject *Second);

  static SAGEExpr getNaN(SAGEInterface &SI);
  static SAGEExpr getPlusInf(SAGEInterface &SI);
  static SAGEExpr getMinusInf(SAGEInterface &SI);
  static SAGEExpr getTrue(SAGEInterface &SI);
  static SAGEExpr getFalse(SAGEInterface &SI);

  int compare(const SAGEExpr& Other) const;

  friend raw_ostream& operator<<(raw_ostream& OS, const SAGEExpr& SE);

private:
  SAGEInterface &SI_;
  PyObject *Expr_;
};

namespace std {
  template <>
  struct less<SAGEExpr> {
    bool operator() (const SAGEExpr& LHS, const SAGEExpr& RHS) const {
      return LHS.compare(RHS) < 0;
    }
  };
}

#endif

