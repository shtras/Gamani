#pragma once
#include "AstralBody.h"
#include "DynamicBody.h"

class Star: public DynamicBody
{
public:
  Star();
  ~Star();

  bool isStatic() {return false;}
  void render();
};

