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

void KillRotProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
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
    //cout << "A: " << getShip()->getYaw() << endl;
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

CString RotateProg::getInfo()
{
  return (rotateLeft_?CString("L"):CString("R")) + CString(tgtYaw_, 2) + " " + CString(getShip()->getYaw(),2) + " " + CString(lastDelta_, 2);
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
  target_ = autopilot->getGravityRef();
  if (!target_) {
    autopilot_->setError("No ref defined");
    return;
  }
  //target_ = getShip()->getGravityRef();
  Vector3 tgtCoord = target_->getCoord();
  Vector3 shipCoord = getShip()->getCoord();
  double dist = (tgtCoord - shipCoord).getLength();
  double timeToDist = sqrt(dist/2.0*1e6 / getShip()->gatMarchPower()) * 2;
  //timeToDist *= 0.1;
  Vector3 tgtNewCord = (target_->getCoord()*1e6 + target_->getVelocity()*timeToDist) * 1e-6;

  //tgtCoord = tgtNewCord;
  
  Vector3 dir = tgtCoord - shipCoord;
  double targetAngle = dir.getAngle();

  Vector3 relSpd = getShip()->getVelocity() - target_->getVelocity();
  double relSpdAngle = relSpd.getAngle();

  cout << "tgt " << targetAngle << " spd " << relSpdAngle << endl;

  autopilot_->addProgram(new RotateProg(autopilot_, relSpdAngle - 180.0));
  autopilot_->addProgram(new EqSpeedProg(autopilot_, target_->getVelocity()));
  
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new ApproachProgStep2(autopilot_));

//   autopilot_->addProgram(new AccelProg(autopilot_, (shipCoord + tgtCoord)*0.5));
//   autopilot_->addProgram(new RotateProg(autopilot_, targetAngle + 180.0));
//   autopilot_->addProgram(new AccelProg(autopilot_, tgtCoord));
}

void ApproachProg::step()
{
  endProgram();
}

void ApproachProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
}

//////////////////////////////////////////////////////////////////////////

ApproachProgStep2::ApproachProgStep2(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::Approach;
}

void ApproachProgStep2::step()
{
  endProgram();
}

void ApproachProgStep2::init()
{
  if (init_) {
    return;
  }
  init_ = true;


  target_ = autopilot_->getGravityRef();
  if (!target_) {
    autopilot_->setError("No ref defined");
    return;
  }

  Vector3 tgtCoord = target_->getCoord();
  Vector3 shipCoord = getShip()->getCoord();
  double dist = (tgtCoord - shipCoord).getLength();
  double timeToDist = sqrt(dist/2.0*1e6 / getShip()->gatMarchPower()) * 2;
  //timeToDist *= 0.1;
  Vector3 tgtNewCord = (target_->getCoord()*1e6 + target_->getVelocity()*timeToDist) * 1e-6;

  //tgtCoord = tgtNewCord;

  Vector3 dir = tgtCoord - shipCoord;
  double targetAngle = dir.getAngle();

  Vector3 relSpd = getShip()->getVelocity() - target_->getVelocity();
  double relSpdAngle = relSpd.getAngle();

  cout << "tgt " << targetAngle << " spd " << relSpdAngle << endl;
  autopilot_->addProgram(new AccelProg(autopilot_, (shipCoord + tgtCoord)*0.5));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle + 180.0));
  autopilot_->addProgram(new AccelProg(autopilot_, tgtCoord));

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

void LaunchProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
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

CString AccelProg::getInfo()
{
  return (approaching_?CString("Y"):CString("N")) + Renderer::getInstance().formatDistance(lastDist_);
}

void AccelProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
}

//////////////////////////////////////////////////////////////////////////

EqSpeedProg::EqSpeedProg(Autopilot* autopilot, Vector3 spdToReach):APProgram(autopilot), spdToReach_(spdToReach)
{
  id_ = Autopilot::EqSpeed;
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

void EqSpeedProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  Vector3 vel = getShip()->getVelocity();
  lastDelta_ = (vel - spdToReach_).getLength();
}

//////////////////////////////////////////////////////////////////////////

ProGradeProg::ProGradeProg(Autopilot* autopilot):APProgram(autopilot)
{

}

void ProGradeProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  AstralBody* tgt = autopilot_->getGravityRef();
  if (!tgt) {
    autopilot_->setError("No ref defined");
    return;
  }
  Vector3 dir = tgt->getCoord() - autopilot_->getShip()->getCoord();
  double angle = dir.getAngle();
  autopilot_->addProgram(new RotateProg(autopilot_, angle + 90.0));
}

void ProGradeProg::step()
{
  endProgram();
}

//////////////////////////////////////////////////////////////////////////

RetroGradeProg::RetroGradeProg(Autopilot* autopilot):APProgram(autopilot)
{

}

void RetroGradeProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  AstralBody* tgt = autopilot_->getGravityRef();
  if (!tgt) {
    autopilot_->setError("No ref defined");
    return;
  }
  Vector3 dir = tgt->getCoord() - autopilot_->getShip()->getCoord();
  double angle = dir.getAngle();
  autopilot_->addProgram(new RotateProg(autopilot_, angle - 90.0));
}

void RetroGradeProg::step()
{
  endProgram();
}
