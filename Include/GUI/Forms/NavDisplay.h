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

  void toggleAutoRef();
  void setGravityRef(AstralBody* body) {gravityRef_ = body;}
  void setManualRef(bool value) {manualRef_ = value;}
  AstralBody* getGravityRef() {return gravityRef_;}
protected:
  enum Mode {Orbit, SyncOrbit, Landing, Docking, Axes};
  void modeButtonClick();
  void refButtonClick();
  void autoRefButtonClick();
  void menuModeClick(void* val);
  void tgtToSyncOrbitButtonClick();

  void selectTargetStartingFrom(void* bodyRef);
  void setTarget(void* tgt);

  void drawOrbit(Ship* ship);
  void drawSyncOrbit(Ship* ship, DynamicBody* ref);
  void drawDocking();
  void drawAxes();

  void updateGravityRef();


  WText* shipYawText_;
  WText* gravityRefName_;
  WText* velData_;
  WText* distData_;
  WText* surfDistData_;
  WText* modeName_;
  WButton* modeButton_;
  WButton* refButton_;
  WButton* autoRefButton_;
  WButton* axesButton_;
  WButton* tgtToSyncOrbitButton_;
  WMenu* modeMenu_;
  WMenu* tgtSelectMenu_;

  Mode mode_;
  bool selectRef_;

  AstralBody* syncOrbitRef_;
  AstralBody* gravityRef_;

  bool manualRef_;
};
