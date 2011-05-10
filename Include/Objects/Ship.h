#pragma once
#include "DynamicBody.h"
#include "ModelRenderable.h"

class HUD;

class Ship: public DynamicBody, public ModelRenderable
{
  friend class Gamani;
public:
  enum Autopilot {NoProgram, KillRot, Rotate, Launch, Approach, Accelerate, Stop};
  Ship();
  ~Ship();
  void render();
  void yawLeft();
  void yawLeft(double val);
  void yawRight();
  void yawRight(double val);
  void accelerate();
  void autopilotStep();
  void setAutopilotTo(Autopilot program);
  void collideWith(AstralBody* body);
  bool isLanded() {return landed_;}
  AstralBody* getRefBody() {return landedOn_;}
  Vector3 getLandedCoords();
  Autopilot getProgram() {return currProg_;}
  void setGravityRef(AstralBody* body) {gravityRef_ = body;}
  void setHUD(HUD* hud);
  AstralBody* getGravityRef() {return gravityRef_;}
  void toggleAutoRef();
  void scrollGravityRef();
  double getTgtAngle() {return targetAngle_;}
  double getTgtDist() {return targetDist_;}
  CString getProgName(Autopilot prog);
public:
  void testNavCom();
  void testNavCom1();
  void updateGravityRef();
  vector<Vector3> calcOrbit(AstralBody* from, AstralBody* to, double& minDist, double& maxDist);
  double yawPower_;
  double maxYawVel_;
  double maxSpeed_;
  double marchPower_;
  Autopilot currProg_;
  double targetAngle_;
  double startAngle_;
  bool rotateRight_;
  bool rotatePhase1_;
  double targetDist_;
  Vector3 targetCoords_;

  bool landed_;
  AstralBody* landedOn_;
  AstralBody* gravityRef_;
  int launchCount_;

  HUD* hud_;

  AstralBody* test1_;
  AstralBody* test2_;

  bool manualRef_;
  unsigned int refIdx_;

  list<Autopilot> programs_;
};

