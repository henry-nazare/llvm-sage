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
  return SAGERange(getLower() + Other.getLower(),
                   getUpper() + Other.getUpper());
}

SAGERange SAGERange::operator-(const SAGERange& Other) const {
  return SAGERange(getLower() - Other.getUpper(),
                   getUpper() - Other.getLower());
}

SAGERange SAGERange::operator*(const SAGERange& Other) const {
  SAGEExpr LL = getLower() * Other.getLower(),
           LU = getLower() * Other.getUpper(),
           UL = getUpper() * Other.getLower(),
           UU = getUpper() * Other.getUpper();
  return SAGERange(LL.min(LU).min(UL).min(UU), LL.max(LU).max(UL).max(UU));
}

SAGERange SAGERange::operator/(const SAGERange& Other) const {
  SAGEExpr LL = getLower()/Other.getLower(),
           LU = getLower()/Other.getUpper(),
           UL = getUpper()/Other.getLower(),
           UU = getUpper()/Other.getUpper();
  return SAGERange(LL.min(LU).min(UL).min(UU), LL.max(LU).max(UL).max(UU));
}

bool SAGERange::operator==(const SAGERange& Other) const {
  return getLower().isEQ(Other.getLower())
      && getUpper().isEQ(Other.getUpper());
}

bool SAGERange::operator!=(const SAGERange& Other) const {
  return !(*this == Other);
}

