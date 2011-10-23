#pragma once

class MissionManager
{
public:
  static MissionManager& getInstance();
private:
  MissionManager();
  ~MissionManager();
};
