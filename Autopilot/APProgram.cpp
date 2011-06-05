#include "StdAfx.h"
#include "APProgram.h"
#include "Ship.h"
#include "Renderer.h"
#include "Gamani.h"

APProgram::APProgram(Autopilot* autopilot):autopilot_(autopilot), id_(Autopilot::UnKnown),init_(false)
{

}

APProgram::~APProgram()
{

}

Ship* APProgram::getShip()
{
  return autopilot_->getShip();
}

void APProgram::endProgram()
{
  autopilot_->endCurrProg();
}

//////////////////////////////////////////////////////////////////////////

KillRotProg::KillRotProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::KillRot;
}

void KillRotProg::step()
{
  Ship* ship = getShip();
  double yawVel = ship->getYawVel();
  double yawPower = ship->getYawPower();
  if (abs(yawVel) < yawPower/10.0f) {
    ship->setYawVel(0);
    endProgram();
  } else if (yawVel > 0) {
    ship->yawLeft();
  } else {
    ship->yawRight();
  }
}

//////////////////////////////////////////////////////////////////////////

RotateProg::RotateProg(Autopilot* autopilot, double tgtYaw):APProgram(autopilot),tgtYaw_(tgtYaw)
{
  id_ = Autopilot::Rotate;
  while (tgtYaw_ > 360.0) {
    tgtYaw_ -= 360.0;
  }
  while (tgtYaw_ < 0) {
    tgtYaw_ += 360.0;
  }
}

void RotateProg::step()
{
  if (!init_) {
    init();
  }
  double currDelta = getShip()->getYaw() - tgtYaw_;
  while (currDelta > 180.0) {
    currDelta -= 180.0;
  }
  while (currDelta < 0) {
    currDelta += 180.0;
  }
  
  if ((rotateLeft_ && (currDelta > lastDelta_)) || (!rotateLeft_ && (currDelta < lastDelta_))) {
    if (rotateLeft_) {
      getShip()->yawRight();
    } else {
      getShip()->yawLeft();
    }
    cout << "A: " << getShip()->getYaw() << endl;
    endProgram();
    autopilot_->addImmediateProgram(new KillRotProg(autopilot_));
    return;
  }
  lastDelta_ = currDelta;

  if (!rotateLeft_) {
    getShip()->yawRight();
  } else {
    getShip()->yawLeft();
  }
}

void RotateProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;

  startYaw_ = getShip()->getYaw();

  double deltaYaw = tgtYaw_ - startYaw_;
  while (deltaYaw < 0) {
    deltaYaw += 360;
  }
  tgtYaw_ =  startYaw_ + deltaYaw / 2.0;
  while (tgtYaw_ > 360) {
    tgtYaw_ -= 360;
  }
  if (deltaYaw > 180) {
    rotateLeft_ = true;
  } else {
    rotateLeft_ = false;
  }
  lastDelta_ = startYaw_ - tgtYaw_;
  while (lastDelta_ > 180.0) {
    lastDelta_ -= 180.0;
  }
  while (lastDelta_ < 0) {
    lastDelta_ += 180.0;
  }
}

//////////////////////////////////////////////////////////////////////////

ApproachProg::ApproachProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::Approach;
  //target_ = getShip()->getGravityRef();
  Vector3 tgtCoord = target_->getCoord();
  Vector3 shipCoord = getShip()->getCoord();
  double dist = (tgtCoord - shipCoord).getLength();
  double timeToDist = sqrt(dist/2.0*1e6 / getShip()->gatMarchPower()) * 2;
  //timeToDist *= 0.1;
  Vector3 tgtNewCord = (target_->getCoord()*1e6 + target_->getVelocity()*timeToDist) * 1e-6;

  tgtCoord = tgtNewCord;
  
  Vector3 dir = tgtCoord - shipCoord;
  
  double targetAngle = acos(dir[0]/dir[1]) * 180.0 / 3.14159265 - 90;
  if (fabs(dir[1]) < 0.1) {
    targetAngle = (dir[1] > 0)? 270 : 90;
  }

  Vector3 relSpd = target_->getVelocity() - getShip()->getVelocity();
  double relSpdAngle = acos(relSpd[0]/relSpd[1]) * 180.0 / 3.14159265 - 90;

  autopilot_->addProgram(new RotateProg(autopilot_, relSpdAngle - 180.0));
  autopilot_->addProgram(new EqSpeedProg(autopilot_, target_->getVelocity()));
  
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));

  autopilot_->addProgram(new AccelProg(autopilot_, (shipCoord + tgtCoord)*0.5));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle + 180.0));
  autopilot_->addProgram(new AccelProg(autopilot_, tgtCoord));
}

void ApproachProg::step()
{
  endProgram();
}

//////////////////////////////////////////////////////////////////////////

LaunchProg::LaunchProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::Launch;
  launchingFrom_ = getShip()->getLandedOn();
}

void LaunchProg::step()
{
  if (!launchingFrom_) {
    endProgram();
    return;
  }
  getShip()->accelerate();
  Vector3 shipVel = getShip()->getVelocity();
  Vector3 launchPadVel = launchingFrom_->getVelocity();
  Vector3 relSpd = shipVel - launchPadVel;
  double relSpdVal = relSpd.getLength();
  if (relSpdVal > 1000) {
    endProgram();
  }
}

//////////////////////////////////////////////////////////////////////////

AccelProg::AccelProg(Autopilot* autopilot, Vector3 coordsToStop):APProgram(autopilot), coordsToStop_(coordsToStop)
{
  id_ = Autopilot::Accelerate;
  lastDist_ = (getShip()->getCoord() - coordsToStop_).getLength();
  approaching_ = false;
}

void AccelProg::step()
{
  getShip()->accelerate();
  double currDist = (getShip()->getCoord() - coordsToStop_).getLength();
  if (!init_) {
    lastDist_ = currDist;
    init_ = true;
    return;
  }
  assert(currDist > 0);

  if (!approaching_ && (currDist < lastDist_)) {
    approaching_ = true;
  }
  if (approaching_ && (currDist > lastDist_)) {
    endProgram();
  }
  lastDist_ = currDist;
}

//////////////////////////////////////////////////////////////////////////

EqSpeedProg::EqSpeedProg(Autopilot* autopilot, Vector3 spdToReach):APProgram(autopilot), spdToReach_(spdToReach)
{
  Vector3 vel = getShip()->getVelocity();
  lastDelta_ = (vel - spdToReach).getLength();
}

void EqSpeedProg::step()
{
  getShip()->accelerate();
  Vector3 vel = getShip()->getVelocity();
  double delta = (vel - spdToReach_).getLength();
  if (delta > lastDelta_) {
    endProgram();
  }
  lastDelta_ = delta;
}
