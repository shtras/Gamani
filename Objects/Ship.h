#pragma once
#include "DynamicBody.h"
#include "ModelRenderable.h"
#include "Autopilot.h"
#include "Dockable.h"
#include "Station.h"
#include "Planet.h"

class HUD;

/*
Engines:
      5
    /---\
   2|   |4
    |   |
   1|   |3
    /---\
      0
*/

class Ship: public DynamicBody, public ModelRenderable, public Dockable
{
  friend class Gamani;
public:
  enum Engine {March = 0, RearLeft, FrontLeft, RearRight, FrontRight, Reverse};
  Ship();
  ~Ship();
  void render();
  void yawLeft();
  void yawLeft(double val);
  void yawRight();
  void yawRight(double val);
  void accelerate(double fraction = 1.0);
  void back(double fraction = 1.0);
  void steerLeft(double fraction = 1.0);
  void steerRight(double fraction = 1.0);
  void autopilotStep();
  void setAutopilotTo(Autopilot::ProgID program);
  void collideWith(AstralBody* body);
  bool isLanded() {return landed_;}
  bool isDocked() {return docked_;}
  AstralBody* getRefBody() {return landedOn_;}
  Vector3 getLandedCoords();
  Vector3 getLandedVel();
  //void setGravityRef(AstralBody* body) {gravityRef_ = body;}
  void setHUD(HUD* hud);
  //AstralBody* getGravityRef() {return gravityRef_;}
  void toggleAutoRef();
  //void scrollGravityRef();
  CString getCurrProgName();
  void setYawPower(double yawPower) {yawPower_ = yawPower;}
  double getYawPower() const {return yawPower_;}
  double getMarchPower() {return marchPower_;}
  AstralBody* getLandedOn() const {return landedOn_;}
  bool isLaunching();
  void setDockedCoord();
  //void setManualRef(bool value) {manualRef_ = value;}
  void undock();
  void checkUndocking();
  bool isUndocking() {return !docked_ && dockedTo_;}
  Autopilot* getAP() {return autopilot_;}
  void initializeAsPlayerControlled() {initializeAsPlayerControlled_ = true;}
  bool isInitializedAsPlayerControlled() {return initializeAsPlayerControlled_;}
  void engageEngine(Engine engine, float power = 1.0);
  void updateEngines();
  void drawEngines();
  Station* getDockedTo() {return dockedTo_;}
  HUD* getHUD() {return hud_;}
  void setInAtmosphere(Planet* planet) {inAtmosphereOf_ = planet;}
  Planet* getInAtmosphere() {return inAtmosphereOf_;}
public:
  //void testNavCom();
  //void testNavCom1();
  //void updateGravityRef();
  void attemptDocking(Station* station);
  vector<Vector3> calcOrbit(AstralBody* from, AstralBody* to, double& minDist, double& maxDist);
  double yawPower_;
  double maxYawVel_;
  double maxSpeed_;
  double marchPower_;

  Autopilot* autopilot_;

  bool landed_;
  bool docked_;
  AstralBody* landedOn_;
  //AstralBody* gravityRef_;
  int launchCount_;

  HUD* hud_;

  AstralBody* test1_;
  AstralBody* test2_;

  //bool manualRef_;
  unsigned int refIdx_;
  Station* dockedTo_;
  float engineStates_[6];

  bool initializeAsPlayerControlled_;
  Planet* inAtmosphereOf_;
};

