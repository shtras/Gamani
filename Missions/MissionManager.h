#pragma once
#include "MissionDisplay.h"
#include "Mission.h"
#include "Station.h"
#include "LayoutManager.h"

class MissionManager
{
public:
  static MissionManager& getInstance();
  void init(LayoutManager* layoutManager);
  void addMission(Mission* mission);
  void removeMission(Mission* mission);
  void addTask(int missionID, Task* task);
  void testInit(Station* station);
  Mission* getMissionByID(int id);
  void setShip(Ship* ship) {ship_ = ship;}
  Ship* getShip() {return ship_;}
  void notifyUpdated(int missionID);

  void checkCompleted();
  void reset();

  signal1<WLayout*> AddLayout;
  signal1<WLayout*> RemoveLayout;
private:
  MissionManager();
  ~MissionManager();

  MissionDisplay* display_;
  map<int, Mission*> missionsMap_;
  list<Mission*> missions_;

  Ship* ship_;
};
