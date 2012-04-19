#pragma once

class Operand;
class AModeOperand;
class FPROperand;
class Amode;
class GPR;
class FPR;

class Instruction
{
  Instruction(char opcode);
  virtual ~Instruction();
private:
  char opcode_;
};

class D_S_Form: public Instruction
{
public:
  D_S_Form();
  ~D_S_Form();
private:
  AModeOperand* dest_;
  AModeOperand* src_;
  bool shiftedSrc_;
};

class D_Form: public Instruction
{
public:
  D_Form();
  ~D_Form();
private:
  AModeOperand* dest_;
};

class F_F_Form: public Instruction
{
public:
  F_F_Form();
  ~F_F_Form();
private:
  FPROperand* dest_;
  FPROperand* src_;
};

class G_F_Form: public Instruction
{
public:
  G_F_Form();
  ~G_F_Form();
private:
  AModeOperand* dest_;
  FPROperand* src_;
};

class F_G_Form: public Instruction
{
public:
  F_G_Form();
  ~F_G_Form();
private:
  FPROperand* dest_;
  AModeOperand* src_;
};

class Jcc_Form: public Instruction
{
public:
  Jcc_Form();
  ~Jcc_Form();
private:
};

class Operand
{
public:
  Operand();
  virtual ~Operand();
private:
};

class AModeOperand: public Operand
{
public:
  AModeOperand();
  ~AModeOperand();
private:
  Amode* amode_;
  GPR* gpr_;
};

class FPROperand: public Operand
{
public:
  FPROperand();
  ~FPROperand();
private:
  FPR* fpr_;
};

class Amode
{
public:
  Amode(int num);
  ~Amode();
private:
  int num_;
};

class GPR
{
public:
  GPR(int num);
  ~GPR();
private:
  int num_;
};

class FPR
{
public:
  FPR(int num);
  ~FPR();
private:
  int num_;
};
