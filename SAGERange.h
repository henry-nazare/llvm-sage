#ifndef _SAGERANGE_H_
#define _SAGERANGE_H_

#include "SAGEExpr.h"

class SAGERange {
public:
  SAGERange(SAGEExpr Both);
  SAGERange(SAGEExpr Lower, SAGEExpr Upper);

	SAGEExpr getLower() const { return Lower_; }
	SAGEExpr getUpper() const { return Upper_; }

	void setLower(const SAGEExpr& Lower) { Lower_ = Lower; }
	void setUpper(const SAGEExpr& Upper) { Upper_ = Upper; }

  SAGERange operator+(const SAGERange& Other) const;
  SAGERange operator-(const SAGERange& Other) const;
  SAGERange operator*(const SAGERange& Other) const;
  SAGERange operator/(const SAGERange& Other) const;

	bool operator==(const SAGERange& Other) const;
	bool operator!=(const SAGERange& Other) const;

private:
	SAGEExpr Lower_;
	SAGEExpr Upper_;
};

raw_ostream& operator<<(raw_ostream& OS, const SAGERange& R);

#endif

