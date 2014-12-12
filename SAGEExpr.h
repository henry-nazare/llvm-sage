#ifndef _SAGEEXPR_H_
#define _SAGEEXPR_H_

#include "SAGEInterface.h"

using namespace llvm;

class SAGEExpr {
public:
  SAGEExpr(SAGEInterface &SI);
  SAGEExpr(SAGEInterface &SI, int Int);
  SAGEExpr(SAGEInterface &SI, APInt Int);
  SAGEExpr(SAGEInterface &SI, const Twine &Name);
  SAGEExpr(SAGEInterface &SI, PyObject *Obj);
  SAGEExpr(SAGEInterface &SI, const Value *V);

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

  void operator=(SAGEExpr& Other);
  void operator=(const SAGEExpr& Other);

  bool isEQ(const SAGEExpr &Other) const;
  bool isNE(const SAGEExpr &Other) const;

  bool isConstant() const;

  static SAGEExpr getNaN(SAGEInterface &SI);
  static SAGEExpr getPlusInf(SAGEInterface &SI);
  static SAGEExpr getMinusInf(SAGEInterface &SI);
  static SAGEExpr getTrue(SAGEInterface &SI);
  static SAGEExpr getFalse(SAGEInterface &SI);

  int compare(const SAGEExpr& Other) const;

  friend raw_ostream& operator<<(raw_ostream& OS, const SAGEExpr& SE);

protected:
  PyObject *getExpr() const;

private:
  SAGEInterface &SI_;
  PyObject *Expr_;
};

template <>
struct std::less<SAGEExpr> {
  bool operator() (const SAGEExpr& LHS, const SAGEExpr& RHS) const {
    return LHS.compare(RHS) < 0;
  }
};

#endif

