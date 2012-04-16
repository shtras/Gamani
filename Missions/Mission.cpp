#include "StdAfx.h"
#include "Mission.h"
#include "MissionManager.h"
#include "Ship.h"

int Mission::LastID = 0;
int Task::LastID = 0;

Mission::Mission():description_(""),completed_(false),visible_(true),finalized_(false)
{
  id_ = LastID++;
}

Mission::~Mission()
{

}

void Mission::addTask(Task* task)
{
  tasks_.push_back(task);
  MissionManager::getInstance().addTask(id_, task);
}

void Mission::checkCompleted()
{
  if (completed_) {
    return;
  }
  bool res = true;
  for (auto itr = tasks_.begin(); itr != tasks_.end(); ++itr) {
    Task* task = *itr;
    if (task->isCompletedNotCheck()) {
      continue;
    }
    if (task->isCompleted()) {
      MissionManager::getInstance().notifyUpdated(id_);
    } else {
      res = false;
    }
  }
  completed_ = res;
}

bool Mission::isCompleted()
{
  if (!completed_) {
    checkCompleted();
  }
  return completed_;
}

void Mission::finalize()
{
  if (finalized_) {
    return;
  }
  finalized_ = true;
  MissionManager::getInstance().notifyUpdated(id_);
  for (auto itr=addOnComplete_.begin(); itr != addOnComplete_.end(); ++itr) {
    MissionManager::getInstance().addMission(*itr);
  }
  for (auto itr=removeOnComplete_.begin(); itr != removeOnComplete_.end(); ++itr) {
    MissionManager::getInstance().removeMission(*itr);
  }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Task::Task():target_(NULL), targetDist_(0), num_(0), type_(Unknown),completed_(false)
{
  id_ = LastID++;
}

Task::~Task()
{

}

void Task::setDependsOn(Task* task)
{
  assert (task != this);
  dependsOn_.push_back(task);
}

bool Task::isCompleted()
{
  if (!completed_) {
    checkCompletion();
  }
  return completed_;
}

void Task::checkCompletion()
{
  assert(!completed_);
  for (auto itr = dependsOn_.begin(); itr != dependsOn_.end(); ++itr) {
    Task* dependsOn = *itr;
    if (!dependsOn->isCompleted()) {
      return;
    }
  }
  bool res = false;
  switch(type_) {
  case Approach:
    res = checkApproachCompleted();
    break;
  case Land:
    res = checkLandCompleted();
    break;
  case Dock:
    res = checkDockCompleted();
    break;
  case Orbit:
    res = checkOrbitCompleted();
    break;
  case MoveFrom:
    res = checkMoveFromCompleted();
    break;
  default:
    assert(0);
  }
  if (res) {
    completed_ = true;    
  }
}

/////////////////////////////////////////////////////////////////////////
///////////// Polymorhpism requested!!! Monkeycode detected /////////////
/////////////////////////////////////////////////////////////////////////
bool Task::checkApproachCompleted()
{
  Ship* ship = MissionManager::getInstance().getShip();
  Vector3 dir = ship->getCoord() - target_->getCoord();
  double dist = dir.getLength() - target_->getRadius();
  if (dist <= targetDist_) {
    return true;
  }
  return false;
}

bool Task::checkLandCompleted()
{
  Ship* ship = MissionManager::getInstance().getShip();
  if (ship->isLanded() && ship->getLandedOn() == target_) {
    return true;
  }
  return false;
}

bool Task::checkDockCompleted()
{
  Ship* ship = MissionManager::getInstance().getShip();
  if (ship->isDocked() && ship->getDockedTo() == target_) {
    return true;
  }
  return false;
}

bool Task::checkOrbitCompleted()
{
  Ship* ship = MissionManager::getInstance().getShip();

  Vector3 r = (ship->getCoord() - target_->getCoord()) * 1e6;
  Vector3 v = (ship->getVelocity() - target_->getVelocity());
  double M = target_->getMass() * 1e6;
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


  double eps = 1.0-(b*b/(a*a));
  if (eps > 0) {
    eps = sqrt(eps);
  } else {
    eps = 0;
  }

  double dp = a*(1-eps) * 1e-6;
  double da = a*(1+eps) * 1e-6;

  if (da >= targetDist_ && dp >= targetDist_) {
    return true;
  }

  return false;
}

bool Task::checkMoveFromCompleted()
{
  Ship* ship = MissionManager::getInstance().getShip();
  Vector3 dir = ship->getCoord() - target_->getCoord();
  double dist = dir.getLength() - target_->getRadius();
  if (dist > targetDist_) {
    return true;
  }
  return false;
}
