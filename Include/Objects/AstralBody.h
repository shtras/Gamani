#pragma once
#include "Renderable.h"

class AstralBody: public Renderable
{
public:
  AstralBody();
  AstralBody(double x, double y, double z);
  virtual ~AstralBody();
  virtual void render();
  double getMass() {return mass_;}
  void setMass(double mass) {mass_ = mass;}
  void setRadius(double radius) {radius_ = radius;}
  bool isStatic() {return true;}
  Vector3 getCoord() {return coord_;}
  void setCoord(Vector3 coord) {coord_ = coord;}
  void setColor(Vector3 color) {color_ = color;}
  virtual Vector3 getVelocity() {return Vector3(0,0,0);}
  virtual void setVelocity(Vector3 vel) {assert(0);}
  virtual double getRadius() {return radius_;}
  virtual void collideWith(AstralBody* body) {/*assert(0);*/}
  CString getName() {return name_;}
  void setName(CString name) {name_ = name;}
  vector<AstralBody*>& getSatellites() {return satellites_;}
  void addSatellite(AstralBody* satellite);
  bool removeSatellite(AstralBody* satellite);
  void rotationStep();
  void setRotationPeriod(double speed); //Period in seconds to make 360 degrees turn
  double getRotationAngleSpeed(); //Angle per second rotation
  void drawName();
protected:
  double radius_;
  Vector3 coord_;
  double mass_;
  Vector3 color_;
  CString name_;

  double rotation_;
  double rotationSpeed_;

  vector<AstralBody*> satellites_;
};

