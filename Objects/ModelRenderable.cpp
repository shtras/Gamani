#include "StdAfx.h"
#include "ModelRenderable.h"
#include "Wrapper3DS.h"

ModelRenderable::ModelRenderable():model_(NULL)
{

}

ModelRenderable::~ModelRenderable()
{
  delete model_;
}

void ModelRenderable::loadModelFromFile(CString fileName)
{
  model_ = Wrapper3DS::getInstance().Load(fileName);
  if (!model_) {
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Model not loaded: ") + fileName);
    return;
  }
  //model_->normalize();
  model_->initVBOs();
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
