#ifndef _SAGEINTERFACE_H_
#define _SAGEINTERFACE_H_

#include "Python/PythonInterface.h"

#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <vector>

#undef NAN

using namespace llvm;

class SAGEInterface : public ModulePass {
public:
  static char ID;
  SAGEInterface() : ModulePass(ID) { }

  enum {
    FN_EXPR_INIT,
    FN_NAN,
    FN_PLUS_INF,
    FN_MINUS_INF,
    FN_TRUE,
    FN_FALSE
  };

  static std::vector<PythonInterface::PythonObjInfo> getObjVecInit() {
    return {
      // FN_EXPR_INIT
      PythonInterface::PythonObjInfo("llvmsage.expr", "Expr", nullptr),
      // FN_NAN
      PythonInterface::PythonObjInfo("llvmsage.expr", "Expr", "get_nan"),
      // FN_PLUS_INF
      PythonInterface::PythonObjInfo("llvmsage.expr", "Expr", "get_plus_inf"),
      // FN_MINUS_INF
      PythonInterface::PythonObjInfo("llvmsage.expr", "Expr", "get_minus_inf"),
      // FN_TRUE
      PythonInterface::PythonObjInfo("llvmsage.expr", "Expr", "get_true"),
      // FN_FALSE
      PythonInterface::PythonObjInfo("llvmsage.expr", "Expr", "get_false")
    };
  }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual bool doFinalization(Module&);
  virtual bool runOnModule(Module&);

  PyObject *var(const long Val);
  PyObject *var(const unsigned long Val);
  PyObject *var(const char *Str);

  std::string getName(PyObject *Expr);

  PyObject *add(PyObject *LHS, PyObject *RHS);
  PyObject *sub(PyObject *LHS, PyObject *RHS);
  PyObject *mul(PyObject *LHS, PyObject *RHS);
  PyObject *div(PyObject *LHS, PyObject *RHS);
  PyObject *pow(PyObject *Base, PyObject *Exp);
  PyObject *neg(PyObject *Expr);

  PyObject *eq(PyObject *LHS, PyObject *RHS);
  PyObject *ne(PyObject *LHS, PyObject *RHS);
  PyObject *lt(PyObject *LHS, PyObject *RHS);
  PyObject *le(PyObject *LHS, PyObject *RHS);
  PyObject *gt(PyObject *LHS, PyObject *RHS);
  PyObject *ge(PyObject *LHS, PyObject *RHS);

  PyObject *_and(PyObject *LHS, PyObject *RHS);
  PyObject *_or(PyObject *LHS, PyObject *RHS);
  PyObject *_not(PyObject *Expr);

  PyObject *min(PyObject *First, PyObject *Second);
  PyObject *min(PyObject *First, PyObject *Second, PyObject *Assumptions);
  PyObject *max(PyObject *First, PyObject *Second);
  PyObject *max(PyObject *First, PyObject *Second, PyObject *Assumptions);

  std::vector<PyObject*> args(PyObject *Expr);

  PyObject *getNaN();
  PyObject *getPlusInf();
  PyObject *getMinusInf();
  PyObject *getTrue();
  PyObject *getFalse();

  long getInteger(PyObject *Expr);
  long getSize(PyObject *Expr);

  PyObject *getNumer(PyObject *Expr);
  PyObject *getDenom(PyObject *Expr);
  PyObject *getBase(PyObject *Expr);
  PyObject *getExp(PyObject *Expr);

  bool isEQ(PyObject *First, PyObject *Second);
  bool isNE(PyObject *First, PyObject *Second);

  bool isConstant(PyObject *Expr);
  bool isInteger(PyObject *Expr);
  bool isRational(PyObject *Expr);
  bool isSymbol(PyObject *Expr);

  bool isMin(PyObject *Expr);
  bool isMax(PyObject *Expr);

  bool isAdd(PyObject *Expr);
  bool isMul(PyObject *Expr);
  bool isPow(PyObject *Expr);

  bool isInf(PyObject *Expr);
  bool isPlusInf(PyObject *Expr);
  bool isMinusInf(PyObject *Expr);

  int compare(PyObject *First, PyObject *Second);

private:
  PythonInterface *PI_;
  std::unique_ptr<PythonInterface::PythonObjVec> ObjVec_;
};

#endif

