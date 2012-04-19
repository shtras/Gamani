#pragma once
#include "ADPISA.h"

class ADPFactory
{
public:
  static ADPFactory& getInstance()
  {
    static ADPFactory instance;
    return instance;
  }

  Instruction* createInstr(CString& opcode, CString& op1, CString& op2);
private:
  ADPFactory();
  ~ADPFactory();
};
