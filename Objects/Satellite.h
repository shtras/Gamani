#pragma once
#include "DynamicBody.h"
#include "ModelRenderable.h"

class Satellite: public DynamicBody, public ModelRenderable
{
public:
  Satellite();
  ~Satellite();
  void render();
protected:
};
