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
  // FIXME: Not always valid.
  return SAGERange(getLower() * Other.getUpper(),
                   getUpper() * Other.getLower());
}

SAGERange SAGERange::operator/(const SAGERange& Other) const {
  // FIXME: Not always valid.
  return SAGERange(getLower()/Other.getUpper(),
                   getUpper()/Other.getLower());
}

bool SAGERange::operator==(const SAGERange& Other) const {
  return getLower().isEQ(Other.getLower())
      && getUpper().isEQ(Other.getUpper());
}

bool SAGERange::operator!=(const SAGERange& Other) const {
  return !(*this == Other);
}

