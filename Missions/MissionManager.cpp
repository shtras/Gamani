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

void MissionManager::addMission(Mission* mission)
{
  missions_.push_back(mission);
  missionsMap_[mission->getID()] = mission;
  display_->notifyChanged(mission->getID(), MissionDisplay::Added);
}

void MissionManager::removeMission(Mission* mission)
{
  display_->notifyChanged(mission->getID(), MissionDisplay::Removed);
  missions_.remove(mission);
  missionsMap_.erase(mission->getID());
  delete mission;
}

Mission* MissionManager::getMissionByID(int id)
{
  assert (missionsMap_.count(id) == 1);
  return missionsMap_[id];
}

void MissionManager::addTask(int missionID, Task* task)
{
  if (missionsMap_.count(missionID) > 0) {
    display_->notifyChanged(missionID, MissionDisplay::Changed);
  }
}

void MissionManager::init(LayoutManager* layoutManager)
{
  AddLayout.connect(layoutManager, &LayoutManager::addLayout);
  RemoveLayout.connect(layoutManager, &LayoutManager::removeLayout);

  TextPanel* textPanel = new TextPanel();
  textPanel->init();
  textPanel->setDimensions(0.31, 0.9, 0.35, 0.4);
  textPanel->setMinimizible(false);
  AddLayout(textPanel);

  display_ = new MissionDisplay(textPanel);
  display_->init();
  display_->setDimensions(0, 0.9, 0.3, 0.4);
  AddLayout(display_);


}

void MissionManager::testInit(Station* station)
{
  vector<CString> fullDescr;
  AstralBody* earth = Gamani::getInstance().getWorld()->getObject("Earth");
  AstralBody* jupiter = Gamani::getInstance().getWorld()->getObject("Jupiter");
  AstralBody* mars = Gamani::getInstance().getWorld()->getObject("Mars");
  AstralBody* hawkeye = Gamani::getInstance().getWorld()->getObject("Hawkeye");
  Mission* launchMission = new Mission();
  launchMission->setDescription("Into space!");
  fullDescr.push_back("Hi there! How's your new ship? Now you take good care of her,");
  fullDescr.push_back("Will ya? Make yourself comfortable and get used to the controls");
  fullDescr.push_back("and then come up here. I'm waiting for you on the space station");
  fullDescr.push_back("named Shipyard. I'll guide you step by step, so don't you worry");
  launchMission->setFullDescr(fullDescr);
  fullDescr.clear();
  

  Task* launchTask = new Task();
  launchTask->setDescription("Perform launch");
  launchTask->setType(Task::MoveFrom);
  launchTask->setTarget(earth);
  launchTask->setDist(2);
  fullDescr.push_back("First you need to unpause and launch from the planet.");
  fullDescr.push_back("Use the 'Launch' autopilot command, wait for it to complete");
  fullDescr.push_back("then use acceleration to climb to at least 2000km from surface.");
  launchTask->setFullDescr(fullDescr);
  launchMission->addTask(launchTask);
  fullDescr.clear();

  Task* orbitTask = new Task();
  orbitTask->setDescription("Acquire orbit");
  fullDescr.push_back("The next step is to acquire a stable");
  fullDescr.push_back("orbit with both Apoapsis and Periapsis");
  fullDescr.push_back("of at least 8600km");
  orbitTask->setFullDescr(fullDescr);
  orbitTask->setType(Task::Orbit);
  orbitTask->setTarget(earth);
  orbitTask->setDist(8.6);
  orbitTask->setDependsOn(launchTask);
  launchMission->addTask(orbitTask);
  fullDescr.clear();

  Task* dockTask = new Task();
  dockTask->setDescription("Dock at Shipyard");
  fullDescr.push_back("The last step is to dock at the");
  fullDescr.push_back("station named Shipyard.");
  fullDescr.push_back("Synchronize your orbit and use the communication");
  fullDescr.push_back("display to request docking permission.");
  fullDescr.push_back("The docking course will be shown.");
  fullDescr.push_back("Good luck!");
  dockTask->setFullDescr(fullDescr);
  dockTask->setType(Task::Dock);
  dockTask->setTarget(station);
  dockTask->setDependsOn(orbitTask);
  launchMission->addTask(dockTask);
  fullDescr.clear();
  
  addMission(launchMission);


  Mission* toJupiterMission = new Mission();
  toJupiterMission->setDescription("Get to Jupiter");
  fullDescr.push_back("Your mission is to get to Jupiter and perform some reckon.");
  fullDescr.push_back("You will need to get as close as 10000km and acquire orbit.");
  toJupiterMission->setFullDescr(fullDescr);
  fullDescr.clear();
  
  Task* getToJupiterTask = new Task();
  getToJupiterTask->setDescription("Come close to Jupiter");
  fullDescr.push_back("You will need to approach Jupiter.");
  fullDescr.push_back("Get as close as 100'000km from the surface");
  fullDescr.push_back("Make sure you don't smash into the planet");
  fullDescr.push_back("This ship is quite expensive, you know");
  getToJupiterTask->setFullDescr(fullDescr);
  getToJupiterTask->setType(Task::Approach);
  getToJupiterTask->setTarget(jupiter);
  getToJupiterTask->setDist(jupiter->getRadius() + 100);
  toJupiterMission->addTask(getToJupiterTask);
  fullDescr.clear();

  Task* orbitJupiterTask = new Task();
  orbitJupiterTask->setDescription("Acquire orbit");
  fullDescr.push_back("As you may have already guessed, the next");
  fullDescr.push_back("step will be to acquire a stable orbit");
  fullDescr.push_back("with at least 110000km Apoapsis and Periapsis");
  fullDescr.push_back("We'll get in touch with you when you're done");
  orbitJupiterTask->setFullDescr(fullDescr);
  orbitJupiterTask->setType(Task::Orbit);
  orbitJupiterTask->setTarget(jupiter);
  orbitJupiterTask->setDist(110);
  orbitJupiterTask->setDependsOn(getToJupiterTask);
  toJupiterMission->addTask(orbitJupiterTask);
  fullDescr.clear();
  launchMission->addMissionToAddOnComplete(toJupiterMission);

  Mission* toMarsMission = new Mission();
  toMarsMission->setDescription("To Mars!");
  fullDescr.push_back("Scrub your current mission, we have an");
  fullDescr.push_back("important task for you!");
  fullDescr.push_back("Some of our guys were sent to Mars a while ago.");
  fullDescr.push_back("They use an old rocket engine ship and guess what!");
  fullDescr.push_back("The idiots used their fuel to cook Turkey");
  fullDescr.push_back("for Thanksgiving and are now all");
  fullDescr.push_back("And are now all out of fuel.");
  fullDescr.push_back("Get there ASAP and help them!");
  toMarsMission->setFullDescr(fullDescr);
  fullDescr.clear();

  Task* getToMars = new Task();
  getToMars->setDescription("Get to Mars");
  fullDescr.push_back("You know the drill. Get to at least 40'000km from Mars");
  getToMars->setFullDescr(fullDescr);
  getToMars->setType(Task::Approach);
  getToMars->setTarget(mars);
  getToMars->setDist(mars->getRadius() + 40);
  toMarsMission->addTask(getToMars);
  fullDescr.clear();

  Task* toHawkeye = new Task();
  toHawkeye->setDescription("Find Hawkeye");
  fullDescr.push_back("Their ship name is Hawkeye. You must find it");
  fullDescr.push_back("And get as close as possible. But not closer than 500m");
  fullDescr.push_back("You don't want your family to pay for two ships, do you?");
  toHawkeye->setFullDescr(fullDescr);
  toHawkeye->setType(Task::Approach);
  toHawkeye->setTarget(hawkeye);
  toHawkeye->setDist(0.005);
  toHawkeye->setDependsOn(getToMars);
  toMarsMission->addTask(toHawkeye);
  fullDescr.clear();

  Mission* invisMission = new Mission();
  invisMission->setVisible(false);
  addMission(invisMission);
  Task* invisTask = new Task();
  invisTask->setType(Task::MoveFrom);
  invisTask->setTarget(earth);
  invisTask->setDist(1000);
  invisMission->addTask(invisTask);
  invisMission->addMissionToRemoveOnComplete(toJupiterMission);
  invisMission->addMissionToAddOnComplete(toMarsMission);
}

void MissionManager::notifyUpdated(int missionID)
{
  display_->notifyChanged(missionID, MissionDisplay::Changed);
}

void MissionManager::checkCompleted()
{
  for (auto itr = missions_.begin(); itr != missions_.end(); ++itr) {
    Mission* mission = *itr;
    if (mission->isCompleted()) {
      mission->finalize();
    }
  }
}

void MissionManager::reset()
{
  AddLayout.disconnect_all();
  RemoveLayout.disconnect_all();
  auto itr = missions_.begin();
  for (; itr != missions_.end(); ++itr) {
    Mission* mission = *itr;
    delete mission;
  }
  missions_.clear();
  missionsMap_.clear();
  //delete display_;
  display_ = NULL;
}
