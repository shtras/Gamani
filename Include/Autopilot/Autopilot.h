#pragma once

#include "AstralBody.h"

class Ship;
class APProgram;

class Autopilot
{
public:
  enum ProgID {NoProgram, KillRot, Rotate, Launch, Approach, Accelerate, Stop, EqSpeed, ProGrade, RetroGrade, UnKnown};
  Autopilot(Ship* ship);
  ~Autopilot();

  void step();
  Ship* getShip() {return ship_;}
  void clearQueue();
  void addProgram(APProgram* prog);
  void addImmediateProgram(APProgram* prog);
  void endCurrProg();
  ProgID getCurrProg();
  CString getProgInfo();
  AstralBody* getGravityRef() {return ref_;}
  void setGravityRef(AstralBody* body) {ref_ = body;}
  void setError(CString newError) {lastError_ = newError;}
  CString getError() {return lastError_;}
private:
  void GC();
  Ship* ship_;

  AstralBody* ref_;

  list <APProgram*> programs_;
  list <APProgram*> garbage_;

  CString lastError_;
};
