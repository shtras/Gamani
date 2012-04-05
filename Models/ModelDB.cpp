#include "StdAfx.h"
#include "ModelDB.h"
#include "Wrapper3DS.h"

ModelDB::ModelDB()
{

}

ModelDB::~ModelDB()
{
  auto itr = models_.begin();
  for (; itr != models_.end(); ++itr) {
    Model* model = (*itr).second;
    delete model;
  }
}

Model* ModelDB::getModel(CString fileName)
{
  Model* res = NULL;
  if (models_.count(fileName) == 0) {
    res = Wrapper3DS::getInstance().Load(fileName);
    res->initVBOs();
    models_[fileName] = res;
  } else {
    res = models_[fileName];
  }
  return res;
}
