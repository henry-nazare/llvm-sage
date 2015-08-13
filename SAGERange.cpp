//===---------------------------- SAGERange.cpp ---------------------------===//
//===----------------------------------------------------------------------===//

#include "SAGERange.h"

using namespace llvm;

raw_ostream& operator<<(raw_ostream& OS, const SAGERange &R) {
  OS << "[" << R.getLower() << ", " << R.getUpper() << "]";
  return OS;
}

SAGERange::SAGERange(SAGEExpr Both)
  : Lower_(Both), Upper_(Both) {
}

SAGERange::SAGERange(SAGEExpr Lower, SAGEExpr Upper)
  : Lower_(Lower), Upper_(Upper) {
}

SAGERange SAGERange::operator+(const SAGERange& Other) const {
  bool lowerShouldBeInf = getLower().isMinusInf()
      || Other.getLower().isMinusInf();
  bool upperShouldBeInf = getUpper().isPlusInf()
      || Other.getUpper().isPlusInf();
  auto& SI = getLower().getSI();
  return SAGERange(lowerShouldBeInf ? SAGEExpr::getMinusInf(SI) : getLower() + Other.getLower(),
      upperShouldBeInf ? SAGEExpr::getPlusInf(SI) : getUpper() + Other.getUpper());
}

SAGERange SAGERange::operator-(const SAGERange& Other) const {
  bool lowerShouldBeInf = getLower().isMinusInf()
      || Other.getLower().isMinusInf();
  bool upperShouldBeInf = getUpper().isPlusInf()
      || Other.getUpper().isPlusInf();
  auto& SI = getLower().getSI();
  return SAGERange(lowerShouldBeInf ? SAGEExpr::getMinusInf(SI) : getLower() - Other.getLower(),
      upperShouldBeInf ? SAGEExpr::getPlusInf(SI) : getUpper() - Other.getUpper());
}

SAGERange SAGERange::operator*(const SAGERange& Other) const {
  bool lowerShouldBeInf = getLower().isMinusInf()
      || Other.getLower().isMinusInf();
  bool upperShouldBeInf = getUpper().isPlusInf()
      || Other.getUpper().isPlusInf();
  auto& SI = getLower().getSI();
  SAGEExpr LL = getLower() * Other.getLower(),
           LU = getLower() * Other.getUpper(),
           UL = getUpper() * Other.getLower(),
           UU = getUpper() * Other.getUpper();
  return SAGERange(lowerShouldBeInf ? SAGEExpr::getMinusInf(SI) : LL.min(LU).min(UL).min(UU),
       upperShouldBeInf ? SAGEExpr::getPlusInf(SI) : LL.max(LU).max(UL).max(UU));
}

SAGERange SAGERange::operator/(const SAGERange& Other) const {
  bool lowerShouldBeInf = getLower().isMinusInf()
      || Other.getLower().isMinusInf();
  bool upperShouldBeInf = getUpper().isPlusInf()
      || Other.getUpper().isPlusInf();
  auto& SI = getLower().getSI();
  SAGEExpr LL = getLower()/Other.getLower(),
           LU = getLower()/Other.getUpper(),
           UL = getUpper()/Other.getLower(),
           UU = getUpper()/Other.getUpper();
  return SAGERange(lowerShouldBeInf ? SAGEExpr::getMinusInf(SI) : LL.min(LU).min(UL).min(UU),
       upperShouldBeInf ? SAGEExpr::getPlusInf(SI) : LL.max(LU).max(UL).max(UU));
}

bool SAGERange::operator==(const SAGERange& Other) const {
  return getLower().isEQ(Other.getLower())
      && getUpper().isEQ(Other.getUpper());
}

bool SAGERange::operator!=(const SAGERange& Other) const {
  return !(*this == Other);
}

