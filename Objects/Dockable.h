#pragma once
#include "Vector.h"

class Dockable
{
public:
  Dockable():dockingPort_(Vector3()){}
  ~Dockable(){}
  Vector3 getDockingPort() {return dockingPort_;}
  void setDockingPort(Vector3 dockingPort) {dockingPort_ = dockingPort;}
  double getPortAngle() {return portAngle_;}
  void setPortAngle(double portAngle) {portAngle_ = portAngle;}
protected:
  void renderPort();
private:
  Vector3 dockingPort_;
  double portAngle_;
};
