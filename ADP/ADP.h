#pragma once
#include "ADPFactory.h"

class ADP
{
public:
  ADP();
  ~ADP();

  void reset();
  bool loadProgram(char* prog, int size);
  bool step();
  bool isRunning() {return running_;}
private:
  map<int, Instruction*> instrCache_;
  char* memory_;
  int gpr_[16];
  double fpr_[15];
  int psw_;

  bool running_;
};
