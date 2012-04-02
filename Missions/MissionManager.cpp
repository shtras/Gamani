#include "StdAfx.h"
#include "MissionManager.h"
#include "Gamani.h"

MissionManager& MissionManager::getInstance()
{
  static MissionManager instance;
  return instance;
}

MissionManager::MissionManager():
  display_(NULL)
{

}

MissionManager::~MissionManager()
{

}

void MissionManager::setDisplay(MissionDisplay* display)
{
  assert(!display_);
  display_ = display;
}

void MissionManager::addMission(Mission* mission)
{
  missions_.push_back(mission);
  missionsMap_[mission->getID()] = mission;
  display_->notifyChanged(mission->getID(), MissionDisplay::Added);
}

Mission* MissionManager::getMissionByID(int id)
{
  assert (missionsMap_.count(id) == 1);
  return missionsMap_[id];
}

void MissionManager::addTask(int missionID, Task* task)
{
  display_->notifyChanged(missionID, MissionDisplay::Changed);
}

void MissionManager::testInit(Station* station)
{
  AstralBody* earth = Gamani::getInstance().getWorld()->getObject("Earth");
  Mission* newMission = new Mission();
  newMission->setDescription("Test mission");
  addMission(newMission);
  Task* newTask = new Task();
  newTask->setDescription("Dock to the station");
  newTask->setTarget(station);
  newTask->setType(Task::Dock);
  newMission->addTask(newTask);
  Task* newTask2 = new Task();
  newTask2->setDescription("Land on Earth");
  newTask2->setTarget(earth);
  newTask2->setType(Task::Land);
  newTask2->setDependsOn(newTask);
  newMission->addTask(newTask2);

  newMission = new Mission();
  newMission->setDescription("Test sequence");
  addMission(newMission);
  newTask = new Task();
  newTask->setDescription("Land on earth");
  newTask->setTarget(earth);
  newTask->setType(Task::Land);
  newMission->addTask(newTask);

  Task* newTask1 = new Task();
  newTask1->setDescription("Dock to station");
  newTask1->setTarget(station);
  newTask1->setType(Task::Dock);
  newTask1->setDependsOn(newTask);
  newMission->addTask(newTask1);
}

void MissionManager::notifyUpdated(int missionID)
{
  display_->notifyChanged(missionID, MissionDisplay::Changed);
}

void MissionManager::checkCompleted()
{
  for (auto itr = missions_.begin(); itr != missions_.end(); ++itr) {
    Mission* mission = *itr;
    mission->checkCompleted();
  }
}
