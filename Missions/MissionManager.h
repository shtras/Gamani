#pragma once
#include "MissionDisplay.h"
#include "Mission.h"
#include "Station.h"

class MissionManager
{
public:
  static MissionManager& getInstance();
  void setDisplay(MissionDisplay* display);
  void addMission(Mission* mission);
  void addTask(int missionID, Task* task);
  void testInit(Station* station);
  Mission* getMissionByID(int id);
  void setShip(Ship* ship) {ship_ = ship;}
  Ship* getShip() {return ship_;}
  void notifyUpdated(int missionID);

  void checkCompleted();
private:
  MissionManager();
  ~MissionManager();

  MissionDisplay* display_;
  map<int, Mission*> missionsMap_;
  list<Mission*> missions_;

  Ship* ship_;
};
