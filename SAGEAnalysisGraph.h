#ifndef _SAGEANALYSISGRAPH_H_
#define _SAGEANALYSISGRAPH_H_

// Should always be the first include.
#include "Python/PythonInterface.h"

#include "SAGENameVault.h"

#include "llvm/IR/Value.h"

#include <map>

using namespace llvm;

class SAGEAnalysisGraph : public llvmpy::PyObjectHolder {
public:
  SAGEAnalysisGraph(PyObject *Graph, SAGENameVault &SNV);

protected:
  virtual void setNode(Value *V, PyObject *Node);
  virtual PyObject *getNode(Value *V);
  virtual PyObject *getNodeName(Value *V) const;

  void solve() const;

  template <typename T>
  void addIncoming(iterator_range<T> RangeFrom, Value *To) {
    for (auto &From : RangeFrom) {
      addIncoming(From, To);
    }
  }

  void addIncoming(Value *V, Value *Incoming);
  void addIncoming(PyObject *Node, PyObject *Incoming) const;

protected:
  SAGENameVault &SNV_;

  std::map<Value*, PyObject*> Node_;
};

#endif

