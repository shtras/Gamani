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
};

class RotateProg: public APProgram
{
public:
  RotateProg(Autopilot* autopilot, double tgtYaw);
  virtual ~RotateProg(){}
  virtual void step();
private:
  void init();
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
private:
  AstralBody* target_;
};

class LaunchProg: public APProgram
{
public:
  LaunchProg(Autopilot* autopilot);
  virtual ~LaunchProg(){}
  virtual void step();
private:
  AstralBody* launchingFrom_;
};

class AccelProg: public APProgram
{
public:
  AccelProg(Autopilot* autopilot, Vector3 coordsToStop);
  virtual ~AccelProg(){}
  virtual void step();
private:
  Vector3 coordsToStop_;
  double lastDist_;
  bool approaching_;
};

class EqSpeedProg: public APProgram
{
public:
  EqSpeedProg(Autopilot* autopilot, Vector3 spdToReach);
  virtual ~EqSpeedProg(){}
  virtual void step();
private:
  Vector3 spdToReach_;
  double lastDelta_;
};
