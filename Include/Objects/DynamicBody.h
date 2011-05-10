#pragma once
#include "AstralBody.h"

class DynamicBody: public AstralBody
{
public:
  DynamicBody();
  ~DynamicBody();
  bool isStatic() {return false;}
  Vector3 getVelocity() {return velocity_;}
  void setVelocity(Vector3 vel) {velocity_ = vel;}
  double getYaw() {return yaw_;}
  void setYaw(double yaw) {yaw_ = yaw;}
  double getYawVel() {return yawVel_;}
  void setYawVel(double yawVel) {yawVel_ = yawVel;}
  void render();
  void snapshot();
  virtual AstralBody* getRefBody() {return NULL;}
  virtual Vector3 getLandedCoords() {assert(0); return Vector3();}
protected:
  Vector3 velocity_;
  double yaw_;
  double yawVel_;

  list<Vector3> points_;
};

