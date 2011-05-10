#pragma once
#include "Model.h"

class ModelRenderable
{
public:
  ModelRenderable();
  virtual ~ModelRenderable();
  void initModel(CString fileName);
protected:
  void loadModelFromFile(CString fileName);
  const Model* getModel();
private:
  Model* model_;
  CString modelName_;
};
