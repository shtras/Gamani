#pragma once
#include "WLayout.h"
#include "Mission.h"
#include "WText.h"
#include "WTextBox.h"
#include "TextPanel.h"

class MissionDisplay: public WLayout
{
public:
  enum NotificationType {Added, Removed, Changed};
  MissionDisplay(TextPanel* textPanel);
  ~MissionDisplay();
  void init();
  void render();

  void notifyChanged(int missionID, NotificationType type);
private:
  void updateData(int missionID);
  void addButtons(int missionID);
  void removeButtons(int missionID);

  void expandMissionClick(void* param);
  void expandMission(int missionID);
  void expandTask(void* param);
  void clearTasks();

  void expand(bool value);
  list<WButton*> missionButtons_;
  map<int, WButton*> buttonsMap_;
  list<WButton*> tasks_;
  int displayedMissionID_;

  TextPanel* textPanel_;
};
