#pragma once

class Operand;
class AModeOperand;
class FPROperand;
class Amode;
class GPR;
class FPR;
class Immediate;

class Instruction
{
public:
  Instruction(char opcode);
  virtual ~Instruction();
  virtual bool isMe(char opcode) {return opcode == opcode_;}
  virtual int getSize() = 0;
private:
  char opcode_;
};

class D_S_Form: public Instruction
{
public:
  D_S_Form(char opcode);
  virtual ~D_S_Form();
  virtual int getSize();
private:
  AModeOperand* dest_;
  AModeOperand* src_;
  bool shiftedSrc_;
};

class D_Form: public Instruction
{
public:
  D_Form(char opcode);
  virtual ~D_Form();
  virtual int getSize();
private:
  AModeOperand* dest_;
};

class F_F_Form: public Instruction
{
public:
  F_F_Form(char opcode);
  virtual ~F_F_Form();
  virtual int getSize();
private:
  FPROperand* dest_;
  FPROperand* src_;
};

class G_F_Form: public Instruction
{
public:
  G_F_Form(char opcode);
  virtual ~G_F_Form();
  virtual int getSize();
private:
  AModeOperand* dest_;
  FPROperand* src_;
};

class F_G_Form: public Instruction
{
public:
  F_G_Form(char opcode);
  virtual ~F_G_Form();
  virtual int getSize();
private:
  FPROperand* dest_;
  AModeOperand* src_;
};

class Jcc_Form: public Instruction
{
public:
  Jcc_Form(char opcode);
  virtual ~Jcc_Form();
  virtual int getSize();
private:
};

class Operand
{
public:
  Operand();
  virtual ~Operand();
  virtual int getSize() = 0;
private:
};

class AModeOperand: public Operand
{
public:
  AModeOperand(Amode* amode, GPR* gpr, Immediate* imm);
  virtual ~AModeOperand();
  int getSize();
private:
  Amode* amode_;
  GPR* gpr_;
  Immediate* imm_;
};

class FPROperand: public Operand
{
public:
  FPROperand(FPR* fpr);
  virtual ~FPROperand();
  int getSize();
private:
  FPR* fpr_;
};

class Amode
{
public:
  Amode(int num);
  ~Amode();
  int getSize();
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

class Immediate
{
public:
  Immediate(int val);
  ~Immediate();
private:
  int val_;
};

class FPR
{
public:
  FPR(int num, double fimm);
  ~FPR();
  int getSize();
private:
  int num_;
  double fimm_;
};

class Mov: public D_S_Form
{
public:
  Mov();
  ~Mov();
  static bool isMe(CString& mnemonic) {return mnemonic == "mov";}
};

class Add: public D_S_Form
{
public:
  Add();
  ~Add();
  static bool isMe(CString& mnemonic) {return mnemonic == "add";}
};

class Sub: public D_S_Form
{
public:
  Sub();
  ~Sub();
  static bool isMe(CString& mnemonic) {return mnemonic == "sub";}
};

class And: public D_S_Form
{
public:
  And();
  ~And();
  static bool isMe(CString& mnemonic) {return mnemonic == "and";}
};

class Or: public D_S_Form
{
public:
  Or();
  ~Or();
  static bool isMe(CString& mnemonic) {return mnemonic == "or";}
};

class Not: public D_S_Form
{
public:
  Not();
  ~Not();
  static bool isMe(CString& mnemonic) {return mnemonic == "not";}
};

class Xor: public D_S_Form
{
public:
  Xor();
  ~Xor();
  static bool isMe(CString& mnemonic) {return mnemonic == "xor";}
};

class Asl: public D_S_Form
{
public:
  Asl();
  ~Asl();
  static bool isMe(CString& mnemonic) {return mnemonic == "asl";}
};

class Asr: public D_S_Form
{
public:
  Asr();
  ~Asr();
  static bool isMe(CString& mnemonic) {return mnemonic == "asr";}
};

class Push: public D_Form
{
public:
  Push();
  ~Push();
  static bool isMe(CString& mnemonic) {return mnemonic == "push";}
};

class Pop: public D_Form
{
public:
  Pop();
  ~Pop();
  static bool isMe(CString& mnemonic) {return mnemonic == "pop";}
};

class Lpsw: public D_Form
{
public:
  Lpsw();
  ~Lpsw();
  static bool isMe(CString& mnemonic) {return mnemonic == "lpsw";}
};

class Spsw: public D_Form
{
public:
  Spsw();
  ~Spsw();
  static bool isMe(CString& mnemonic) {return mnemonic == "spsw";}
};

class Cmp: public D_S_Form
{
public:
  Cmp();
  ~Cmp();
  static bool isMe(CString& mnemonic) {return mnemonic == "cmp";}
};

class Jmp: public D_Form
{
public:
  Jmp();
  ~Jmp();
  static bool isMe(CString& mnemonic) {return mnemonic == "jmp";}
};

class Je: public Jcc_Form
{
public:
  Je();
  ~Je();
  static bool isMe(CString& mnemonic) {return mnemonic == "je";}
};

class Jne: public Jcc_Form
{
public:
  Jne();
  ~Jne();
  static bool isMe(CString& mnemonic) {return mnemonic == "jne";}
};

class Jg: public Jcc_Form
{
public:
  Jg();
  ~Jg();
  static bool isMe(CString& mnemonic) {return mnemonic == "jg";}
};

class Jge: public Jcc_Form
{
public:
  Jge();
  ~Jge();
  static bool isMe(CString& mnemonic) {return mnemonic == "jge";}
};

class Call: public D_Form
{
public:
  Call();
  ~Call();
  static bool isMe(CString& mnemonic) {return mnemonic == "call";}
};

class Fmov: public F_F_Form
{
public:
  Fmov();
  ~Fmov();
  static bool isMe(CString& mnemonic) {return mnemonic == "fmov";}
};

class Fadd: public F_F_Form
{
public:
  Fadd();
  ~Fadd();
  static bool isMe(CString& mnemonic) {return mnemonic == "fadd";}
};

class Fsub: public F_F_Form
{
public:
  Fsub();
  ~Fsub();
  static bool isMe(CString& mnemonic) {return mnemonic == "fsub";}
};

class Fmul: public F_F_Form
{
public:
  Fmul();
  ~Fmul();
  static bool isMe(CString& mnemonic) {return mnemonic == "fmul";}
};

class Fdiv: public F_F_Form
{
public:
  Fdiv();
  ~Fdiv();
  static bool isMe(CString& mnemonic) {return mnemonic == "fdiv";}
};

class Fcmp: public F_F_Form
{
public:
  Fcmp();
  ~Fcmp();
  static bool isMe(CString& mnemonic) {return mnemonic == "fcmp";}
};

class FLoad: public F_G_Form
{
public:
  FLoad();
  ~FLoad();
  static bool isMe(CString& mnemonic) {return mnemonic == "fload";}
};

class Fstore: public G_F_Form
{
public:
  Fstore();
  ~Fstore();
  static bool isMe(CString& mnemonic) {return mnemonic == "fstore";}
};

