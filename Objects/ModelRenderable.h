#pragma once
#include "Model.h"

class ModelRenderable
{
public:
  ModelRenderable();
  virtual ~ModelRenderable();
  void initModel(CString fileName);
  const CString& getModelName() {return modelName_;}
protected:
  void loadModelFromFile(CString fileName);
  const Model* getModel();
private:
  Model* model_;
  CString modelName_;
};
