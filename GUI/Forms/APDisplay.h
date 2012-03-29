#pragma once
#include "Autopilot.h"
#include "Display.h"
#include "WText.h"
#include "WButton.h"
#include "NavDisplay.h"
#include "WInput.h"

class APDisplay: public Display
{
public:
  APDisplay(NavDisplay* left, NavDisplay* right);
  ~APDisplay();
  void render();
  void init();
  void updateData();
  void setAP(Autopilot* ap) {myAP_ = ap;}
private:
  void leftRefClick();
  void rightRefClick();
  void killRotClick();
  void rotateClick();
  void launchClick();
  void approachClick();
  void proGradeClick();
  void retroGradeClick();
  Autopilot* myAP_;

  NavDisplay* left_;
  NavDisplay* right_;

  WText* progNameText_;
  WText* refText_;
  WText* statusText_;
  WText* errorText_;
  WText* landedDockedText_;
  WButton* fromLeftRefButton_;
  WButton* fromRightRefButton_;
  WButton* killRotProgButton_;
  WButton* rotateProgButton_;
  WButton* launchProgButton_;
  WButton* approachProgButton_;
  WButton* proGradeButton_;
  WButton* retroGradeButton_;

  WInput* testInput_;
};
