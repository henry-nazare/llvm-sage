#ifndef _SAGERANGE_H_
#define _SAGERANGE_H_

#include "Python/PythonInterface.h"

class SAGERange : public llvmpy::PyObjectHolder {
public:
  SAGERange(PyObject *Obj);
};

#endif

