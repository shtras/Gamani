#pragma once
#include "Vector.h"

class Dockable
{
public:
  Dockable():portDist_(1), portAngle_(0){}
  ~Dockable(){}
  double getPortAngle() {return portAngle_;}
  void setPortAngle(double portAngle) {portAngle_ = portAngle;}
  void setPortDist(double value) {portDist_ = value;}
  double getPortDist() {return portDist_;}
protected:
  void renderPort();
private:
  double portAngle_;
  double portDist_;
};
