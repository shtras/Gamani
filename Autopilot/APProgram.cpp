#include "StdAfx.h"
#include "APProgram.h"
#include "Ship.h"
#include "Renderer.h"
#include "Gamani.h"

APProgram::APProgram(Autopilot* autopilot):autopilot_(autopilot), id_(Autopilot::UnKnown),init_(false),immediate_(false)
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
  init();
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
}

void ApproachProg::step()
{
  if (!init_) {
    init();
  }
  endProgram();

  target_ = autopilot_->getGravityRef();
  if (!target_) {
    autopilot_->setError("No ref defined");
    return;
  }
  Vector3 tgtCoord = target_->getCoord();
  Vector3 shipCoord = getShip()->getCoord();
  double dist = (tgtCoord - shipCoord).getLength();
  double timeToDist = sqrt(dist/2.0*1e6 / getShip()->gatMarchPower()) * 2;
  Vector3 tgtNewCord = (target_->getCoord()*1e6 + target_->getVelocity()*timeToDist) * 1e-6;


  Vector3 dir = tgtCoord - shipCoord;
  double targetAngle = dir.getAngle();

  Vector3 relSpd = getShip()->getVelocity() - target_->getVelocity();
  double relSpdAngle = relSpd.getAngle();
  autopilot_->addProgram(new EqSpeedProg(autopilot_, 3));

  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle));
  autopilot_->addProgram(new ApproachProgStep2(autopilot_));
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

  target_ = autopilot_->getGravityRef();
  if (!target_) {
    autopilot_->setError("No ref defined");
    return;
  }

  Vector3 tgtCoord = target_->getCoord();
  Vector3 shipCoord = getShip()->getCoord();
  double dist = (tgtCoord - shipCoord).getLength() - target_->getRadius()*1.1;
  double timeToDist = sqrt(dist * 1e6 / (getShip()->gatMarchPower()/0.05)) * 2.0;
  double timeToRotate = 6.0; //TODO: Calculate from yawPower
  timeToDist -= timeToRotate;
  if (timeToDist < 0) {
    timeToDist = 0;
  }
  //timeToDist *= 0.1;
  Vector3 tgtNewCord = (target_->getCoord()*1e6 + target_->getVelocity()*timeToDist) * 1e-6;

  //tgtCoord = tgtNewCord;

  Vector3 dir = tgtCoord - shipCoord;
  double targetAngle = dir.getAngle();

  Vector3 relSpd = getShip()->getVelocity() - target_->getVelocity();
  double relSpdAngle = relSpd.getAngle();

  Vector3 halfDist = (shipCoord + tgtCoord)*0.5;
  halfDist += (target_->getVelocity() * (timeToDist/2.0))*1e-6;

  //cout << "tgt " << targetAngle << " spd " << relSpdAngle << endl;
  autopilot_->addProgram(new AccelProg(autopilot_, halfDist));
  autopilot_->addProgram(new RotateProg(autopilot_, targetAngle + 180.0));
  autopilot_->addProgram(new AccelProg(autopilot_, tgtNewCord));
  autopilot_->addProgram(new EqSpeedProg(autopilot_));
}

void ApproachProgStep2::init()
{
  if (init_) {
    return;
  }
  init_ = true;
}

//////////////////////////////////////////////////////////////////////////

LaunchProg::LaunchProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::Launch;
  launchingFrom_ = getShip()->getLandedOn();
}

void LaunchProg::step()
{
  if (!init_) {
    init();
  }
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
  if (!init_) {
    init();
  }
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

EqSpeedProg::EqSpeedProg(Autopilot* autopilot):APProgram(autopilot),accelStarted_(false),maxIterations_(1)
{
  id_ = Autopilot::EqSpeed;
}

EqSpeedProg::EqSpeedProg(Autopilot* autopilot, int maxIterations):APProgram(autopilot),accelStarted_(false),maxIterations_(maxIterations)
{
  id_ = Autopilot::EqSpeed;
}

void EqSpeedProg::step()
{
  if (!init_) {
    init();
  }
  if (!accelStarted_) {
    accelStarted_ = true;
    spdToReach_ = target_->getVelocity();
    Vector3 vel = getShip()->getVelocity();
    lastDelta_ = (vel - spdToReach_).getLength();
  }
  spdToReach_ = target_->getVelocity();
  Vector3 vel = getShip()->getVelocity();
  double delta = (vel - spdToReach_).getLength();
  if (delta > 2) {
    getShip()->accelerate();
  } else {
    getShip()->accelerate(0.2);
  }

  if (delta > lastDelta_) {
    endProgram();
    if ((vel - spdToReach_).getLength() > 1 && maxIterations_ > 1) {
      autopilot_->addImmediateProgram(new EqSpeedProg(autopilot_, maxIterations_ - 1));
    }
  }
  lastDelta_ = delta;
}

void EqSpeedProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  if (maxIterations_ < 1) {
    assert(0);
    endProgram();
  }
  target_ = autopilot_->getGravityRef();
  if (!target_) {
    autopilot_->setError("No ref defined");
    endProgram();
    return;
  }

  spdToReach_ = target_->getVelocity();
  Vector3 tgtCoord = target_->getCoord();
  Vector3 shipCoord = getShip()->getCoord();
  double dist = (tgtCoord - shipCoord).getLength();
  double timeToDist = sqrt(dist/2.0*1e6 / getShip()->gatMarchPower()) * 2;
  Vector3 tgtNewCord = (target_->getCoord()*1e6 + target_->getVelocity()*timeToDist) * 1e-6;
  Vector3 dir = tgtCoord - shipCoord;
  double targetAngle = dir.getAngle();
  Vector3 relSpd = getShip()->getVelocity() - target_->getVelocity();
  double relSpdAngle = relSpd.getAngle();
  autopilot_->addImmediateProgram(new RotateProg(autopilot_, relSpdAngle - 180.0));

  Vector3 vel = getShip()->getVelocity();
  lastDelta_ = (vel - spdToReach_).getLength();
}

//////////////////////////////////////////////////////////////////////////

ProGradeProg::ProGradeProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::ProGrade;
}

void ProGradeProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
}

void ProGradeProg::step()
{
  if (!init_) {
    init();
  }
  endProgram();

  AstralBody* tgt = autopilot_->getGravityRef();
  if (!tgt) {
    autopilot_->setError("No ref defined");
    return;
  }
  Vector3 dir = autopilot_->getShip()->getCoord() - tgt->getCoord();
  Vector3 vel = autopilot_->getShip()->getVelocity() - tgt->getVelocity();
  Vector3 dirToVel = dir + vel.getNormalized();

  double angle = dir.getAngle();
  double angle1 = dirToVel.getAngle();
  double delta = ((angle) < (angle1))?90.0:-90.0;
  if (immediate_) {
    autopilot_->addImmediateProgram(new RotateProg(autopilot_, angle + delta));
  } else {
    autopilot_->addProgram(new RotateProg(autopilot_, angle + delta));
  }
}

//////////////////////////////////////////////////////////////////////////

RetroGradeProg::RetroGradeProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::RetroGrade;
}

void RetroGradeProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
}

void RetroGradeProg::step()
{
  if (!init_) {
    init();
  }
  endProgram();
  AstralBody* tgt = autopilot_->getGravityRef();
  if (!tgt) {
    autopilot_->setError("No ref defined");
    return;
  }
  Vector3 dir = autopilot_->getShip()->getCoord() - tgt->getCoord();
  Vector3 vel = autopilot_->getShip()->getVelocity() - tgt->getVelocity();
  Vector3 dirToVel = dir + vel.getNormalized();

  double angle = dir.getAngle();
  double angle1 = dirToVel.getAngle();
  double delta = ((angle) < (angle1))?90.0:-90.0;
  if (immediate_) {
    autopilot_->addImmediateProgram(new RotateProg(autopilot_, angle - delta));
  } else {
    autopilot_->addProgram(new RotateProg(autopilot_, angle - delta));
  }
}

//////////////////////////////////////////////////////////////////////////

OrbitProg::OrbitProg(Autopilot* autopilot):APProgram(autopilot),targetDAFound_(false)
{
  id_ = Autopilot::Orbit;
}

void OrbitProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  if (!autopilot_->getGravityRef()) {
    autopilot_->setError("No ref defined");
    endProgram();
    return;
  }
  autopilot_->addImmediateProgram(new ProGradeProg(autopilot_));
}

void OrbitProg::step()
{
  if (!init_) {
    init();
  }
  AstralBody* gravityRef = autopilot_->getGravityRef();
  Vector3 dir = gravityRef->getCoord() - getShip()->getCoord();
  dir.normalize();
  Vector3 vel = getShip()->getVelocity() - gravityRef->getVelocity();
  double velVal = vel.getLength();
  vel.normalize();
  double dot = dir.dot(vel);
  if (fabs(dot) > 0.2) {
    if (velVal > 500) {
      autopilot_->addImmediateProgram(new ProGradeProg(autopilot_));
      autopilot_->addImmediateProgram(new PrependicularizeSpeedProg(autopilot_));
      return;
    }
  }

  Vector3 r = (getShip()->getCoord() - gravityRef->getCoord()) * 1e6;
  Vector3 v = (getShip()->getVelocity() - gravityRef->getVelocity());
  double M = gravityRef->getMass() * 1e6;
  Vector3 h = r*v;
  Vector3 n = Vector3(0,1,0)*h;
  double G = 6.6725e-11;
  double myu = G*M;
  Vector3 e = (r*(v.dot(v) - myu/r.getLength()) - v*(r.dot(v)))*(1.0/myu);
  double E = v.dot(v)/2.0 - myu/r.getLength();
  double a = -myu/2.0/E;
  if (a < 0) {
    a = -a;
  }
  double elen = e.getLength();
  double b = sqrt(a*a*(1.0-e.dot(e)));
  double theta = h[2]/h.getLength();
  double tau = acos(n.dot(e) / n.getLength() / e.getLength()) * 180.0 / 3.14159265;
  if (e[1] < 0) {
    tau = 360-tau;
  }

  double ab = a/b;

  double eps = 1.0-(b*b/(a*a));
  if (eps > 0) {
    eps = sqrt(eps);
  } else {
    eps = 0;
  }

  double maxDiameter = 2*a*1e-6;
  double translateDist = eps*a*1e-6;
  double scaleFactor = a/b * maxDiameter / 2.0;

  dp_ = a*(1-eps) * 1e-6;
  da_ = a*(1+eps) * 1e-6;

  if (!targetDAFound_) {
    targetDa_ = da_;
    targetDAFound_ = true;
  }

  if (da_ > targetDa_*1.5) {
    endProgram();
  }

  if (dp_ < targetDa_ * 0.95) {
    getShip()->accelerate();
  } else {
    endProgram();
  }
}

CString OrbitProg::getInfo()
{
  CString res = Renderer::getInstance().formatDistance(targetDa_) + CString(":") + Renderer::getInstance().formatDistance(dp_);
  return res;
}

//////////////////////////////////////////////////////////////////////////

RotateToProg::RotateToProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::RotateTo;
}

void RotateToProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  if (!autopilot_->getGravityRef()) {
    autopilot_->setError("No ref defined");
    endProgram();
    return;
  }
}

void RotateToProg::step()
{
  if (!init_) {
    init();
  }
  endProgram();
  AstralBody* gravityRef = autopilot_->getGravityRef();
  Vector3 dir = getShip()->getCoord() - gravityRef->getCoord();
  double angle = dir.getAngle();
  autopilot_->addImmediateProgram(new RotateProg(autopilot_, angle + 180.0));
}

//////////////////////////////////////////////////////////////////////////

RotateFromProg::RotateFromProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::RotateFrom;
}

void RotateFromProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  if (!autopilot_->getGravityRef()) {
    autopilot_->setError("No ref defined");
    endProgram();
    return;
  }
}

void RotateFromProg::step()
{
  if (!init_) {
    init();
  }
  endProgram();
  AstralBody* gravityRef = autopilot_->getGravityRef();
  Vector3 dir = getShip()->getCoord() - gravityRef->getCoord();
  double angle = dir.getAngle();
  autopilot_->addImmediateProgram(new RotateProg(autopilot_, angle));
}

//////////////////////////////////////////////////////////////////////////

PrependicularizeSpeedProg::PrependicularizeSpeedProg(Autopilot* autopilot):APProgram(autopilot)
{
  id_ = Autopilot::PerpendOrbit;
}

void PrependicularizeSpeedProg::init()
{
  if (init_) {
    return;
  }
  init_ = true;
  if (!autopilot_->getGravityRef()) {
    autopilot_->setError("No ref defined");
    endProgram();
    return;
  }
  AstralBody* gravityRef = autopilot_->getGravityRef();
  Vector3 dir = gravityRef->getCoord() - getShip()->getCoord();
  Vector3 vel = getShip()->getVelocity() - gravityRef->getVelocity();
  dot_ = dir.dot(vel);
  if (dot_ < 0) {
    autopilot_->addImmediateProgram(new RotateToProg(autopilot_));
  } else {
    autopilot_->addImmediateProgram(new RotateFromProg(autopilot_));
  }
}

void PrependicularizeSpeedProg::step()
{
  if (!init_) {
    init();
  }

  AstralBody* gravityRef = autopilot_->getGravityRef();
  Vector3 dir = gravityRef->getCoord() - getShip()->getCoord();
  Vector3 vel = getShip()->getVelocity() - gravityRef->getVelocity();
  double newDot = dir.dot(vel);
  if (newDot * dot_ < 0) {
    endProgram();
  }
  getShip()->accelerate();
}
