#pragma once
#include "Display.h"
#include "NavDisplay.h"

class CommDisplay: public Display
{
public:
  CommDisplay(NavDisplay* left, NavDisplay* right);
  ~CommDisplay();
  void render();
  void init();
  virtual void updateData();
private:
  NavDisplay* left_;
  NavDisplay* right_;

  AstralBody* target_;

  WText* targetName_;
  WButton* fromLeftButton_;
  WButton* fromRightButton_;

  WButton* requestDockingButton_;

  void fromLeftClick();
  void fromRightClick();
  void requestDockingClick();

  void updateTarget();
};
