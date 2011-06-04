#pragma once
#include "DynamicBody.h"
#include "ModelRenderable.h"
#include "Dockable.h"

class Station: public DynamicBody, public ModelRenderable, public Dockable
{
public:
  Station();
  ~Station();
  void render();
protected:
private:
};
