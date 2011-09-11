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
  rotation_ = 0;
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
  rotation_ += rotationSpeed_ * Gamani::getInstance().getSpeedReduce();
  while (rotation_ > 360) {
    rotation_ -= 360;
  }
  while(rotation_ < 0) {
    rotation_ += 360;
  }
}

void AstralBody::setRotationPeriod(double speed)
{

   rotationSpeed_ = 360/speed;
}

double AstralBody::getRotationAngleSpeed()
{
   return rotationSpeed_ * Gamani::getInstance().getSpeedReduce();
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
