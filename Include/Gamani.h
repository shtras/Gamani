#pragma once
#include "World.h"
#include "LayoutManager.h"
#include "UpperPanel.h"

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
  bool getTracers() {return tracers_;}
  bool getAuxPrints() {return auxPrint_;}
  bool getDrawNames() {return names_;}
  bool drawInterface() {return interface_;}

  void speedUp();
  void speedDown();
  void pause();
  bool usingSkyBox1() {return skybox1_;}
  void setSpeed1x() {speed_ = 1;}
  bool drawRelativeOrbits() {return relativeOrbits_;}
  bool rotateCameraWithObject() {return rotateCameraWithObject_;}
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

  bool tracers_;
  bool auxPrint_;
  bool interface_;

  bool lmDown_;
  bool rmDown_;
  bool lmDrag_;
  bool rmDrag_;
  int lmCoord_[2];
  int rmCoord_[2];

  LayoutManager layoutManager_;
  bool auxAxes_;
  bool names_;

  UpperPanel* upperPanel_;
  bool skybox1_;
  bool relativeOrbits_;

  bool rotateCameraWithObject_;
};

