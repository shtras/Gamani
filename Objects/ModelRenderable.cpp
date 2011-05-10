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
