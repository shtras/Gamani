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

  void setAtmRadius(double radius) {atmRadius_ = radius;}
  void setAtmColor(Vector3 color) {atmColor_ = color;}
  double getAtmRadius() {return atmRadius_;}
  Vector3 getAtmColor() {return atmColor_;}
  bool hasAtmosphere() {return atmRadius_ > 0.01;}
private:
  GLUquadric* quadric_;

  Vector3 atmColor_;
  double atmRadius_;
};

