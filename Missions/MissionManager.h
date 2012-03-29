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
  void testInit(Station* station);
private:
  MissionManager();
  ~MissionManager();

  MissionDisplay* display_;
  list<Mission*> missions_;
};
