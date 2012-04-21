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
  Instruction* createInstr(char* mem, int& offset);
private:
  FPROperand* createFPROperand(char* mem, int& offset);
  AModeOperand* createAmodeOperand(char* mem, int& offset);
  Instruction* createD_S_Form(char opcode, char* mem, int& offset);
  Instruction* createD_Form(char opcode, char* mem, int& offset);
  Instruction* createF_F_Form(char opcode, char* mem, int& offset);
  Instruction* createG_F_Form(char opcode, char* mem, int& offset);
  Instruction* createF_G_Form(char opcode, char* mem, int& offset);
  Instruction* createJcc_Form(char opcode, char* mem, int& offset);
  Instruction* createNull_Form(char opcode, char* mem, int& offset);
  AModeOperand* createAmodeOperand(CString& op);
  FPROperand* createFPROperand(CString& op);
  ADPFactory();
  ~ADPFactory();
};
