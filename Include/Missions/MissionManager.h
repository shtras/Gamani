#pragma once
#include "MissionDisplay.h"

class MissionManager
{
public:
  static MissionManager& getInstance();
  void setDisplay(MissionDisplay* display);
private:
  MissionManager();
  ~MissionManager();

  MissionDisplay* display_;
};
