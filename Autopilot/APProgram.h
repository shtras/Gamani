#pragma once
#include "Autopilot.h"
#include "AstralBody.h"

class Ship;

class APProgram
{
public:
  APProgram(Autopilot* autopilot);
  virtual ~APProgram();
  virtual void step() = 0;
  Autopilot::ProgID getID() {return id_;}
  virtual CString getInfo() {return "Executing";}
  virtual void init() = 0;
protected:
  void endProgram();
  Ship* getShip();
  Autopilot* autopilot_;
  Autopilot::ProgID id_;
  bool init_;
};

class KillRotProg: public APProgram
{
public:
  KillRotProg(Autopilot* autopilot);
  virtual ~KillRotProg(){}
  virtual void step();
  virtual void init();
};

class RotateProg: public APProgram
{
public:
  RotateProg(Autopilot* autopilot, double tgtYaw);
  virtual ~RotateProg(){}
  virtual void step();
  virtual CString getInfo();
  virtual void init();
private:
  //void init();
  bool rotateLeft_;
  double tgtYaw_;
  double startYaw_;
  double lastDelta_;
};

class ApproachProg: public APProgram
{
public:
  ApproachProg(Autopilot* autopilot);
  virtual ~ApproachProg(){}
  virtual void step();
  virtual void init();
private:
  AstralBody* target_;
};

class ApproachProgStep2: public APProgram
{
public:
  ApproachProgStep2(Autopilot* autopilot);
  virtual ~ApproachProgStep2(){}
  virtual void step();
  virtual void init();
private:
  AstralBody* target_;
};

class LaunchProg: public APProgram
{
public:
  LaunchProg(Autopilot* autopilot);
  virtual ~LaunchProg(){}
  virtual void step();
  virtual void init();
private:
  AstralBody* launchingFrom_;
};

class AccelProg: public APProgram
{
public:
  AccelProg(Autopilot* autopilot, Vector3 coordsToStop);
  virtual ~AccelProg(){}
  virtual void step();
  virtual CString getInfo();
  virtual void init();
private:
  Vector3 coordsToStop_;
  double lastDist_;
  bool approaching_;
};

class EqSpeedProg: public APProgram
{
public:
  EqSpeedProg(Autopilot* autopilot);
  EqSpeedProg(Autopilot* autopilot, int maxIterations);
  virtual ~EqSpeedProg(){}
  virtual void step();
  virtual void init();
private:
  int maxIterations_;
  Vector3 spdToReach_;
  double lastDelta_;
  AstralBody* target_;
  bool accelStarted_;
};

class ProGradeProg: public APProgram
{
public:
  ProGradeProg(Autopilot* autopilot);
  virtual ~ProGradeProg() {}
  virtual void step();
  virtual void init();
private:
};

class RetroGradeProg: public APProgram
{
public:
  RetroGradeProg(Autopilot* autopilot);
  virtual ~RetroGradeProg() {}
  virtual void step();
  virtual void init();
private:
};
