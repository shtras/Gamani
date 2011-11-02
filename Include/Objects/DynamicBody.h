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
  double getYawVel() {return yawVel_;}
  void setYawVel(double yawVel) {yawVel_ = yawVel;}
  void render();
  void snapshot();
  virtual AstralBody* getRefBody() {return NULL;}
  virtual Vector3 getLandedCoords() {assert(0); return Vector3();}
  void cleatTrack();
  const list<Vector3>& getPoints() const {return points_;}
protected:
  void renderAbsoluteOrbits();
  void renderRelativeOrbits();
  Vector3 velocity_;
  double yawVel_;
  GLuint texture_;

  list<Vector3> points_;
};

