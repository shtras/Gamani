#pragma once
#include "DynamicBody.h"
#include "ModelRenderable.h"

class Station: public DynamicBody, public ModelRenderable
{
public:
  Station();
  ~Station();
  void render();
protected:
private:
};
