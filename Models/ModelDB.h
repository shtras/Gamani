#pragma once
#include "Model.h"

class ModelDB
{
public:
  static ModelDB& getInstance()
  {
    static ModelDB instance;
    return instance;
  }
  Model* getModel(CString fileName);
private:
  ModelDB();
  ~ModelDB();
  map<CString, Model*> models_;
};
