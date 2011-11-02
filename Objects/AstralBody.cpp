#include "StdAfx.h"
#include "AstralBody.h"
#include "Gamani.h"
#include "Renderer.h"

AstralBody::AstralBody()
{
  radius_ = 100;
  coord_[0] = 0;
  coord_[1] = 0;
  coord_[2] = 0;
  mass_ = 1e7;
  color_[0] = 1;
  color_[1] = 1;
  color_[2] = 0;
  name_ = "NoName";
  yaw_ = -20;
  rotationSpeed_ = 0;
  type_ = AstralType;
}

AstralBody::~AstralBody()
{
}

void AstralBody::render()
{

}

void AstralBody::addSatellite(AstralBody* satellite)
{
  satellites_.push_back(satellite);
}

bool AstralBody::removeSatellite(AstralBody* satellite)
{
  //Seems that vector doesn't support deletion from the middle
  assert(0);
  return false;
}


void AstralBody::rotationStep()
{
  yaw_ -= rotationSpeed_ * Gamani::getInstance().getSpeedReduce();
  while (yaw_ > 360) {
    yaw_ -= 360;
  }
  while(yaw_ < 0) {
    yaw_ += 360;
  }
}

void AstralBody::setRotationPeriod(double speed)
{
  rotationSpeed_ = 360/speed;
  //3600*24 sec  -  360 deg
  //
}

double AstralBody::getRotationAngleSpeed()
{
   return rotationSpeed_;
}

double AstralBody::getSurfaceLinearSpeed()
{
  double radAngleSpeed = DegToRad(getRotationAngleSpeed());

  return radAngleSpeed * radius_ * 1e6;
}

void AstralBody::drawName()
{
  if (!Gamani::getInstance().getDrawNames()) {
    return;
  }
  glDisable(GL_DEPTH_TEST);
  Renderer::getInstance().textOutNoMove(0, 0, 0, name_.operator const char *());
  glEnable(GL_DEPTH_TEST);
}
