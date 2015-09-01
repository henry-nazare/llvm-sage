#include "SAGEAnalysisGraph.h"

using namespace llvm;
using llvmpy::Get;

namespace {

struct SAGEAnalysisGraphObjInfo : public PythonObjInfo {
  SAGEAnalysisGraphObjInfo(const char *Fn)
      : PythonObjInfo("llvmanalysis.graph", "Graph", Fn) {}
};

} // end anonymous namespace

SAGEAnalysisGraph::SAGEAnalysisGraph(PyObject *Graph, SAGENameVault &SNV)
    : PyObjectHolder(Graph), SNV_(SNV) {
}

void SAGEAnalysisGraph::solve() const {
  static SAGEAnalysisGraphObjInfo graph_Graph_solve("solve");
  graph_Graph_solve({get()});
}

void SAGEAnalysisGraph::setNode(Value *V, PyObject *Node) {
  Node_[V] = Node;
}

PyObject *SAGEAnalysisGraph::getNode(Value *V) {
  assert(V->getType()->isIntegerTy() && "Value is not an integer");
  auto It = Node_.find(V);

  assert(It != Node_.end() && "Requested value not in node map");
  return It->second;
}

PyObject *SAGEAnalysisGraph::getNodeName(Value *V) const {
  return Get(SNV_.getName(V));
}

void SAGEAnalysisGraph::addIncoming(Value *From, Value *To) {
  addIncoming(getNode(From), getNode(To));
}

void SAGEAnalysisGraph::addIncoming(PyObject *From, PyObject *To) const {
  static SAGEAnalysisGraphObjInfo graph_Graph_add_incoming("add_edge");
  graph_Graph_add_incoming({get(), From, To});
}

