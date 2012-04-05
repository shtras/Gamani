#include "StdAfx.h"
#include "ModelRenderable.h"
#include "Wrapper3DS.h"
#include "ModelDB.h"

ModelRenderable::ModelRenderable():model_(NULL)
{

}

ModelRenderable::~ModelRenderable()
{
  //delete model_;
}

void ModelRenderable::loadModelFromFile(CString fileName)
{
  model_ = ModelDB::getInstance().getModel(fileName);
  if (!model_) {
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Model not loaded: ") + fileName);
    return;
  }
  //model_->normalize();
}

void ModelRenderable::initModel(CString fileName)
{
  modelName_ = fileName;
}

const Model* ModelRenderable::getModel()
{
  if (!model_) {
    loadModelFromFile(modelName_);
  }
  assert(model_);
  return model_;
}
