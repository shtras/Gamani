#pragma once
#include "DynamicBody.h"
class Planet: public DynamicBody
{
public:
  Planet();
  ~Planet();
  void setName(CString name);

  bool isStatic() {return false;}
  void render();
private:
  GLuint texture_;
};

