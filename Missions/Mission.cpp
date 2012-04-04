#include "StdAfx.h"
#include "Mission.h"
#include "MissionManager.h"
#include "Ship.h"

int Mission::LastID = 0;
int Task::LastID = 0;

Mission::Mission():description_(""),completed_(false)
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
    res &= task->isCompleted();
  }
  completed_ = res;
  if (res) {
    MissionManager::getInstance().notifyUpdated(id_);
  }
}

bool Mission::isCompleted()
{
  if (!completed_) {
    checkCompleted();
  }
  return completed_;
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
  assert(0);
  return false;
}
