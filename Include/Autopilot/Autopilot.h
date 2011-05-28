#pragma once

class Ship;
class APProgram;

class Autopilot
{
public:
  enum ProgID {NoProgram, KillRot, Rotate, Launch, Approach, Accelerate, Stop, UnKnown};
  Autopilot(Ship* ship);
  ~Autopilot();

  void step();
  Ship* getShip() {return ship_;}
  void clearQueue();
  void addProgram(APProgram* prog);
  void addImmediateProgram(APProgram* prog);
  void endCurrProg();
  ProgID getCurrProg();
private:
  void GC();
  Ship* ship_;

  list <APProgram*> programs_;
  list <APProgram*> garbage_;
};
