#pragma once
#include "Display.h"
#include "WText.h"
#include "WButton.h"
#include "WMenu.h"

class Ship;

class NavDisplay: public Display
{
public:
  NavDisplay();
  ~NavDisplay();
  void init();
  void updateData();
  void render();
protected:
  enum Mode {Orbit, SyncOrbit, Landing};
  void modeButtonClick();
  void menuModeClick(void* val);
  void tgtToSyncOrbitButtonClick();

  void selectTargetStartingFrom(void* bodyRef);
  void setTarget(void* tgt);

  void drawOrbit(Ship* ship);
  void drawSyncOrbit(Ship* ship, DynamicBody* ref);

  WText* shipNameText_;
  WText* gravityRefName_;
  WText* velData_;
  WText* modeName_;
  WButton* modeButton_;
  WButton* tgtToSyncOrbitButton_;
  WMenu* modeMenu_;
  WMenu* tgtSelectMenu_;

  Mode mode_;

  AstralBody* syncOrbitRef_;
};
