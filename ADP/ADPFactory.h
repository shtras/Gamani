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

  Instruction* createInstr(CString& mnemonic, CString& op1, CString& op2);
  AModeOperand* createAmodeOperand(CString& op);
  FPROperand* createFPROperand(CString& op);
private:
  ADPFactory();
  ~ADPFactory();
};
