#ifndef _SAGEINTERFACEMILP_H_
#define _SAGEINTERFACEMILP_H_

#include "Python/PythonInterface.h"

#include "llvm/Pass.h"

using namespace llvm;

class SAGEInterfaceMILP : public ModulePass {
public:
  static char ID;
  SAGEInterfaceMILP() : ModulePass(ID) { }

  enum {
    FN_MILP_INIT
  };

  static std::vector<PythonInterface::PythonObjInfo> getObjVecInit() {
    return {
      // FN_MILP_INIT
      PythonInterface::PythonObjInfo("llvmsage.milp", "MILP", nullptr),
    };
  }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual bool runOnModule(Module&);

  PyObject *createMILP();

  PyObject *addConstraint(PyObject *MILP, PyObject *Expr);
  PyObject *setObjective(PyObject *MILP, PyObject *Expr);

  PyObject *solve(PyObject *MILP);

private:
  PythonInterface *PI_;
  PythonInterface::PythonObjVec *ObjVec_;
};

#endif

