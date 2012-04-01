#pragma once
#include "WLayout.h"
#include "Mission.h"
#include "WText.h"

class MissionDisplay: public WLayout
{
public:
  enum NotificationType {Added, Removed, Changed};
  MissionDisplay();
  ~MissionDisplay();
  void init();
  void render();

  void notifyChanged(int missionID, NotificationType type);
private:
  void updateData(int missionID);
  void addButtons(int missionID);

  void expandMission(void* param);
  void clearTasks();
  list<WButton*> missionButtons_;
  map<int, WButton*> buttonsMap_;
  list<WText*> tasks_;
  int displayedMissionID_;
};
