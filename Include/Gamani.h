#pragma once
#include "World.h"
#include "LayoutManager.h"

class Gamani
{
public:
  static Gamani& getInstance();
  bool init(HINSTANCE& hIhstance);
  bool run();
  void handleMessage(UINT message,WPARAM wParam,LPARAM lParam);
  const double getSpeed() const {return speed_;}
  Renderable* getFollowedObject() {return world_->getFollowedObject();}
  bool isPaused() {return paused_;}
  World* getWorld() {return  world_;}
  double long getSeconds() {return seconds_;}
  double getSpeedReduce() {return calcStepLength_;}
  double getDTModifier() {return dtModifier_;}
  bool getAuxAxes() {return auxAxes_;}
private:
  void testInit();
  void layoutTest();
  void handlePressedKey(int key);
  void handlePressedKeys();
  Gamani();
  ~Gamani();
  bool mainLoop();
  set<int> pressedKeys_;
  set<int> nonContKeys_;
  World* world_;
  bool paused_;
  double speed_;
  double long seconds_;
  double calcStepLength_;
  double dtModifier_;

  bool lmDown_;
  bool rmDown_;
  bool lmDrag_;
  bool rmDrag_;
  int lmCoord_[2];
  int rmCoord_[2];

  LayoutManager layoutManager_;
  bool auxAxes_;
};

