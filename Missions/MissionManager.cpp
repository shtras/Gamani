#include "StdAfx.h"
#include "MissionManager.h"

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
}

void MissionManager::testInit(Station* station)
{
  Mission* newMission = new Mission();
  newMission->setDescription("Test mission");
  Task* newTask = new Task();
  newTask->setDescription("Dock to the station");
  newTask->setTarget(station);
  newTask->setType(Task::Dock);
  newMission->addTask(newTask);
  addMission(newMission);
}
