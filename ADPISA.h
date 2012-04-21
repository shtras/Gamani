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
  int getCachedSize();
  virtual int getSize() = 0;
  virtual CString toString() = 0;
  virtual CString getMnemonic() = 0;
  int getOpcodeSize() {return 6;}
  virtual void dump(char* mem, int& offset) = 0;
  void dumpOpcode(char* mem, int& offset);
  char getOpcode() {return opcode_;}
  virtual int getFirstImmAddr() {return -1;}
  virtual int getSecondImmAddr() {return -1;}
  virtual bool isJcc() {return false;}
  virtual void setShiftedSrc(bool value) {}
  virtual void emit(char* memory, int* rs, double* fs, int& psw) = 0;
  virtual int* getD(char* memory, int* rs, double* fs, int& psw) {return NULL;}
  virtual int getS(char* memory, int* rs, double* fs, int& psw) {return 0;}
  virtual double* getFD(char* memory, int* rs, double* fs, int& psw) {return NULL;}
  virtual double getFS(char* memory, int* rs, double* fs, int& psw) {return 0;}
private:
  char opcode_;
  int size_;
};

class D_S_Form: public Instruction
{
public:
  D_S_Form(char opcode, AModeOperand* op1, AModeOperand* op2);
  virtual ~D_S_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  virtual int getSecondImmAddr();
  virtual void setShiftedSrc(bool value) {shiftedSrc_ = value;}
  virtual int* getD(char* memory, int* rs, double* fs, int& psw);
  virtual int getS(char* memory, int* rs, double* fs, int& psw);
private:
  AModeOperand* dest_;
  AModeOperand* src_;
  bool shiftedSrc_;
};

class D_Form: public Instruction
{
public:
  D_Form(char opcode, AModeOperand* op);
  virtual ~D_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  virtual int getFirstImmAddr();
  virtual int* getD(char* memory, int* rs, double* fs, int& psw);
  virtual int getS(char* memory, int* rs, double* fs, int& psw);
private:
  AModeOperand* dest_;
};

class F_F_Form: public Instruction
{
public:
  F_F_Form(char opcode, FPROperand* op1, FPROperand* op2);
  virtual ~F_F_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  virtual double* getFD(char* memory, int* rs, double* fs, int& psw);
  virtual double getFS(char* memory, int* rs, double* fs, int& psw);
private:
  FPROperand* dest_;
  FPROperand* src_;
};

class G_F_Form: public Instruction
{
public:
  G_F_Form(char opcode, AModeOperand* op1, FPROperand* op2);
  virtual ~G_F_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  virtual double* getFD(char* memory, int* rs, double* fs, int& psw);
  virtual double getFS(char* memory, int* rs, double* fs, int& psw);
protected:
  AModeOperand* dest_;
  FPROperand* src_;
};

class F_G_Form: public Instruction
{
public:
  F_G_Form(char opcode, FPROperand* op1, AModeOperand* op2);
  virtual ~F_G_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  virtual double* getFD(char* memory, int* rs, double* fs, int& psw);
  virtual double getFS(char* memory, int* rs, double* fs, int& psw);
private:
  FPROperand* dest_;
  AModeOperand* src_;
};

class Jcc_Form: public Instruction
{
public:
  Jcc_Form(char opcode, Immediate* imm);
  virtual ~Jcc_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  virtual int getFirstImmAddr();
  virtual bool isJcc() {return true;}
  virtual int getS(char* memory, int* rs, double* fs, int& psw);
private:
  Immediate* imm_;
};

class Null_Form: public Instruction
{
public:
  Null_Form(char opcode);
  ~Null_Form();
  virtual int getSize();
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
private:
};

class Operand
{
public:
  Operand();
  virtual ~Operand();
  virtual int getSize() = 0;
  virtual CString toString() = 0;
  virtual void dump(char* mem, int& offset) = 0;
private:
};

class AModeOperand: public Operand
{
public:
  AModeOperand(Amode* amode, GPR* gpr, Immediate* imm);
  virtual ~AModeOperand();
  int getSize();
  virtual CString toString();
  Amode* getAmode() {return amode_;}
  GPR* getGPR() {return gpr_;}
  Immediate* getImm() {return imm_;}
  virtual void dump(char* mem, int& offset);
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
  virtual CString toString();
  virtual void dump(char* mem, int& offset);
  FPR* getFPR() {return fpr_;}
private:
  FPR* fpr_;
};

class Amode
{
public:
  Amode(int num);
  ~Amode();
  int getNum() {return num_;}
private:
  int num_;
};

class GPR
{
public:
  GPR(int num);
  ~GPR();
  int getNum() {return num_;}
private:
  int num_;
};

class Immediate
{
public:
  Immediate(int val);
  ~Immediate();
  int getVal() {return val_;}
private:
  int val_;
};

class FPR
{
public:
  FPR(int num, double fimm);
  ~FPR();
  int getNum() {return num_;}
  double getImm() {return fimm_;}
private:
  int num_;
  double fimm_;
};

class Mov: public D_S_Form
{
public:
  Mov(AModeOperand* op1, AModeOperand* op2);
  ~Mov();
  static bool isMe(CString& mnemonic) {return mnemonic == "mov";}
  virtual CString getMnemonic() {return "Mov";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Add: public D_S_Form
{
public:
  Add(AModeOperand* op1, AModeOperand* op2);
  ~Add();
  static bool isMe(CString& mnemonic) {return mnemonic == "add";}
  virtual CString getMnemonic() {return "Add";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Sub: public D_S_Form
{
public:
  Sub(AModeOperand* op1, AModeOperand* op2);
  ~Sub();
  static bool isMe(CString& mnemonic) {return mnemonic == "sub";}
  virtual CString getMnemonic() {return "Sub";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class And: public D_S_Form
{
public:
  And(AModeOperand* op1, AModeOperand* op2);
  ~And();
  static bool isMe(CString& mnemonic) {return mnemonic == "and";}
  virtual CString getMnemonic() {return "And";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Or: public D_S_Form
{
public:
  Or(AModeOperand* op1, AModeOperand* op2);
  ~Or();
  static bool isMe(CString& mnemonic) {return mnemonic == "or";}
  virtual CString getMnemonic() {return "Or";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Not: public D_S_Form
{
public:
  Not(AModeOperand* op1, AModeOperand* op2);
  ~Not();
  static bool isMe(CString& mnemonic) {return mnemonic == "not";}
  virtual CString getMnemonic() {return "Not";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Xor: public D_S_Form
{
public:
  Xor(AModeOperand* op1, AModeOperand* op2);
  ~Xor();
  static bool isMe(CString& mnemonic) {return mnemonic == "xor";}
  virtual CString getMnemonic() {return "Xor";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Asl: public D_S_Form
{
public:
  Asl(AModeOperand* op1, AModeOperand* op2);
  ~Asl();
  static bool isMe(CString& mnemonic) {return mnemonic == "asl";}
  virtual CString getMnemonic() {return "Asl";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Asr: public D_S_Form
{
public:
  Asr(AModeOperand* op1, AModeOperand* op2);
  ~Asr();
  static bool isMe(CString& mnemonic) {return mnemonic == "asr";}
  virtual CString getMnemonic() {return "Asr";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Push: public D_Form
{
public:
  Push(AModeOperand* op);
  ~Push();
  static bool isMe(CString& mnemonic) {return mnemonic == "push";}
  virtual CString getMnemonic() {return "Push";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Pop: public D_Form
{
public:
  Pop(AModeOperand* op);
  ~Pop();
  static bool isMe(CString& mnemonic) {return mnemonic == "pop";}
  virtual CString getMnemonic() {return "Pop";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Lpsw: public D_Form
{
public:
  Lpsw(AModeOperand* op);
  ~Lpsw();
  static bool isMe(CString& mnemonic) {return mnemonic == "lpsw";}
  virtual CString getMnemonic() {return "Lpsw";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Spsw: public D_Form
{
public:
  Spsw(AModeOperand* op);
  ~Spsw();
  static bool isMe(CString& mnemonic) {return mnemonic == "spsw";}
  virtual CString getMnemonic() {return "Spsw";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Cmp: public D_S_Form
{
public:
  Cmp(AModeOperand* op1, AModeOperand* op2);
  ~Cmp();
  static bool isMe(CString& mnemonic) {return mnemonic == "cmp";}
  virtual CString getMnemonic() {return "Cmp";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Jmp: public D_Form
{
public:
  Jmp(AModeOperand* op);
  ~Jmp();
  static bool isMe(CString& mnemonic) {return mnemonic == "jmp";}
  virtual CString getMnemonic() {return "Jmp";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Je: public Jcc_Form
{
public:
  Je(Immediate* imm);
  ~Je();
  static bool isMe(CString& mnemonic) {return mnemonic == "je";}
  virtual CString getMnemonic() {return "Je";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Jne: public Jcc_Form
{
public:
  Jne(Immediate* imm);
  ~Jne();
  static bool isMe(CString& mnemonic) {return mnemonic == "jne";}
  virtual CString getMnemonic() {return "Jne";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Jg: public Jcc_Form
{
public:
  Jg(Immediate* imm);
  ~Jg();
  static bool isMe(CString& mnemonic) {return mnemonic == "jg";}
  virtual CString getMnemonic() {return "Jg";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Jge: public Jcc_Form
{
public:
  Jge(Immediate* imm);
  ~Jge();
  static bool isMe(CString& mnemonic) {return mnemonic == "jge";}
  virtual CString getMnemonic() {return "Jge";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Call: public D_Form
{
public:
  Call(AModeOperand* op);
  ~Call();
  static bool isMe(CString& mnemonic) {return mnemonic == "call";}
  virtual CString getMnemonic() {return "Call";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Ret: public Null_Form
{
public:
  Ret();
  ~Ret();
  static bool isMe(CString& mnemonic) {return mnemonic == "ret";}
  virtual CString getMnemonic() {return "Ret";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fmov: public F_F_Form
{
public:
  Fmov(FPROperand* op1, FPROperand* op2);
  ~Fmov();
  static bool isMe(CString& mnemonic) {return mnemonic == "fmov";}
  virtual CString getMnemonic() {return "Fmov";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fadd: public F_F_Form
{
public:
  Fadd(FPROperand* op1, FPROperand* op2);
  ~Fadd();
  static bool isMe(CString& mnemonic) {return mnemonic == "fadd";}
  virtual CString getMnemonic() {return "Fadd";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fsub: public F_F_Form
{
public:
  Fsub(FPROperand* op1, FPROperand* op2);
  ~Fsub();
  static bool isMe(CString& mnemonic) {return mnemonic == "fsub";}
  virtual CString getMnemonic() {return "Fsub";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fmul: public F_F_Form
{
public:
  Fmul(FPROperand* op1, FPROperand* op2);
  ~Fmul();
  static bool isMe(CString& mnemonic) {return mnemonic == "fmul";}
  virtual CString getMnemonic() {return "Fmul";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fdiv: public F_F_Form
{
public:
  Fdiv(FPROperand* op1, FPROperand* op2);
  ~Fdiv();
  static bool isMe(CString& mnemonic) {return mnemonic == "fdiv";}
  virtual CString getMnemonic() {return "Fdiv";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fcmp: public F_F_Form
{
public:
  Fcmp(FPROperand* op1, FPROperand* op2);
  ~Fcmp();
  static bool isMe(CString& mnemonic) {return mnemonic == "fcmp";}
  virtual CString getMnemonic() {return "Fcmp";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fload: public F_G_Form
{
public:
  Fload(FPROperand* op1, AModeOperand* op2);
  ~Fload();
  static bool isMe(CString& mnemonic) {return mnemonic == "fload";}
  virtual CString getMnemonic() {return "Fload";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

class Fstore: public G_F_Form
{
public:
  Fstore(AModeOperand* op1, FPROperand* op2);
  ~Fstore();
  static bool isMe(CString& mnemonic) {return mnemonic == "fstore";}
  virtual CString getMnemonic() {return "Fstore";}
  virtual void emit(char* memory, int* rs, double* fs, int& psw);
};

