#include "StdAfx.h"
#include "ADPISA.h"

Instruction::Instruction( char opcode ):opcode_(opcode),size_(-1)
{

}

Instruction::~Instruction()
{

}

void Instruction::dumpOpcode(char* mem, int& offset)
{
  char opcode = getOpcode();
  dumpNumber(mem, offset, opcode, 6);
}

int Instruction::getCachedSize()
{
  if (size_ == -1) {
    size_ = getSize();
  }
  return size_;
}

D_S_Form::D_S_Form(char opcode, AModeOperand* op1, AModeOperand* op2):Instruction(opcode), dest_(op1), src_(op2),shiftedSrc_(false)
{

}

D_S_Form::~D_S_Form()
{
  delete dest_;
  delete src_;
}

int D_S_Form::getSize()
{
  int res = getOpcodeSize() + dest_->getSize() + 1 + src_->getSize();
  if (res%8 != 0) {
    res += (8 - res%8);
  }
  return res;
}

CString D_S_Form::toString()
{
  CString res = getMnemonic();
  if (shiftedSrc_) {
    res += CString('s');
  }
  res += CString(" ") + dest_->toString() + CString(", ") + src_->toString();
  return res;
}

void D_S_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  dest_->dump(mem, offset);
  int charNum = offset/8;
  int bitNum = offset%8;
  if (shiftedSrc_) {
    mem[charNum] = setBit(mem[charNum], bitNum);
  } else {
    mem[charNum] = unsetBit(mem[charNum], bitNum);
  }
  ++offset;
  src_->dump(mem, offset);
  int srcAmode = src_->getAmode()->getNum();
  if (offset%8 != 0) {
    int zeros = 8-offset%8;
    dumpNumber(mem, offset, 0, zeros);
  }
}

int D_S_Form::getSecondImmAddr()
{
  return getOpcodeSize() + dest_->getSize() + 1 + 2;
}

int* D_S_Form::getD( char* memory, int* rs, double* fs, int& psw )
{
  switch (dest_->getAmode()->getNum()) {
  case 0:
    return &(rs[dest_->getGPR()->getNum()]);
  case 1:
    return (int*)&(memory[rs[dest_->getGPR()->getNum()]]);
  case 2:
    return (int*)&(memory[dest_->getImm()->getVal() + rs[dest_->getGPR()->getNum()]]);
  case 3:
    assert(0);
  }
  return NULL;
}

int D_S_Form::getS( char* memory, int* rs, double* fs, int& psw )
{
  int res = 0;
  switch (src_->getAmode()->getNum()) {
  case 0:
    res = (rs[src_->getGPR()->getNum()]);
  case 1:
    res = *(int*)(memory + rs[src_->getGPR()->getNum()]);
  case 2:
    res = *(int*)(memory + src_->getImm()->getVal() + rs[src_->getGPR()->getNum()]);
  case 3:
    res = src_->getImm()->getVal();
  }
  if (shiftedSrc_) {
    res <<= 16;
  }
  return res;
}

D_Form::D_Form(char opcode, AModeOperand* op):Instruction(opcode),dest_(op)
{

}

D_Form::~D_Form()
{
  delete dest_;
}

int D_Form::getSize()
{
  int res = getOpcodeSize();
  res += dest_->getSize();
  if (res%8 != 0) {
    res += 8-res%8;
  }
  return res;
}

CString D_Form::toString()
{
  return getMnemonic() + " " + dest_->toString();
}

void D_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  dest_->dump(mem, offset);
  if (offset%8 != 0) {
    int zeros = 8-offset%8;
    dumpNumber(mem, offset, 0, zeros);
  }
}

int D_Form::getFirstImmAddr()
{
  return getOpcodeSize() + 2;
}

int D_Form::getS( char* memory, int* rs, double* fs, int& psw )
{
  switch (dest_->getAmode()->getNum()) {
  case 0:
    return (rs[dest_->getGPR()->getNum()]);
  case 1:
    return *(int*)(memory + rs[dest_->getGPR()->getNum()]);
  case 2:
    return *(int*)(memory + dest_->getImm()->getVal() + rs[dest_->getGPR()->getNum()]);
  case 3:
    return dest_->getImm()->getVal();
  }
  return 0;
}

int* D_Form::getD( char* memory, int* rs, double* fs, int& psw )
{
  assert(dest_->getAmode()->getNum() == 0);
  return &(rs[dest_->getGPR()->getNum()]);
}

F_F_Form::F_F_Form(char opcode, FPROperand* op1, FPROperand* op2):Instruction(opcode),dest_(op1), src_(op2)
{

}

F_F_Form::~F_F_Form()
{
  delete dest_;
  delete src_;
}

int F_F_Form::getSize()
{
  int res = getOpcodeSize() + dest_->getSize() + src_->getSize();
  if (src_->getFPR()->getNum() != 0xf) {
    res += 2;
  }
  return res;
}

CString F_F_Form::toString()
{
  return getMnemonic() + CString(" ") + dest_->toString() + CString(", ") + src_->toString();
}

void F_F_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  dest_->dump(mem, offset);
  src_->dump(mem, offset);
  if (src_->getFPR()->getNum() != 0xf) {
    dumpNumber(mem, offset, 0, 2);
  }
}

double* F_F_Form::getFD( char* memory, int* rs, double* fs, int& psw )
{
  return &(fs[dest_->getFPR()->getNum()]);
}

double F_F_Form::getFS( char* memory, int* rs, double* fs, int& psw )
{
  if (src_->getFPR()->getNum() != 0xf) {
    return fs[src_->getFPR()->getNum()];
  }
  return src_->getFPR()->getImm();
}

G_F_Form::G_F_Form(char opcode, AModeOperand* op1, FPROperand* op2):Instruction(opcode),dest_(op1), src_(op2)
{

}

G_F_Form::~G_F_Form()
{
  delete dest_;
  delete src_;
}

int G_F_Form::getSize()
{
  int res = getOpcodeSize() + dest_->getSize() + src_->getSize();
  if (dest_->getAmode()->getNum() == 2) {
    res += 4;
  }
  return res;
}

CString G_F_Form::toString()
{
  return getMnemonic() + CString(" ") + dest_->toString() + CString(", ") + src_->toString();
}

void G_F_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  dest_->dump(mem, offset);
  src_->dump(mem, offset);
  if (dest_->getAmode()->getNum() == 2) {
    dumpNumber(mem, offset, 0, 4);
  }
}

double* G_F_Form::getFD( char* memory, int* rs, double* fs, int& psw )
{
  switch (dest_->getAmode()->getNum()) {
  case 0:
    return (double*)&(rs[dest_->getGPR()->getNum()]);
  case 1:
    return (double*)(memory + rs[dest_->getGPR()->getNum()]);
  case 2:
    return (double*)(memory + dest_->getImm()->getVal() + rs[dest_->getGPR()->getNum()]);
  case 3:
    assert(0);
  }
  return NULL;
}

double G_F_Form::getFS( char* memory, int* rs, double* fs, int& psw )
{
  if (src_->getFPR()->getNum() != 0xf) {
    return fs[src_->getFPR()->getNum()];
  }
  return src_->getFPR()->getImm();
}

F_G_Form::F_G_Form(char opcode, FPROperand* op1, AModeOperand* op2):Instruction(opcode), dest_(op1), src_(op2)
{

}

F_G_Form::~F_G_Form()
{
  delete dest_;
  delete src_;
}

int F_G_Form::getSize()
{
  return getOpcodeSize() + dest_->getSize() + src_->getSize();
}

CString F_G_Form::toString()
{
  return getMnemonic() + CString(" ") + dest_->toString() + CString(", ") + src_->toString();
}

void F_G_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  dest_->dump(mem, offset);
  src_->dump(mem, offset);
}

double* F_G_Form::getFD( char* memory, int* rs, double* fs, int& psw )
{
  if (dest_->getFPR()->getNum() == 0xf) {
    assert(0);
  }
  return &(fs[dest_->getFPR()->getNum()]);
}

double F_G_Form::getFS( char* memory, int* rs, double* fs, int& psw )
{
  switch (src_->getAmode()->getNum()) {
  case 0:
    return (double)(rs[src_->getGPR()->getNum()]);
  case 1:
    return *(double*)(memory + rs[src_->getGPR()->getNum()]);
  case 2:
    return *(double*)(memory + src_->getImm()->getVal() + rs[src_->getGPR()->getNum()]);
  case 3:
    return (double)src_->getImm()->getVal();
  }
  return 0;
}

Jcc_Form::Jcc_Form(char opcode, Immediate* imm):Instruction(opcode), imm_(imm)
{

}

Jcc_Form::~Jcc_Form()
{
  delete imm_;
}

int Jcc_Form::getSize()
{
  return getOpcodeSize() + 10;
}

CString Jcc_Form::toString()
{
  return getMnemonic() + CString(" ") + CString(imm_->getVal());
}

void Jcc_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  int num = imm_->getVal();
  int n1 = (num & 0x300) >> 8;
  dumpNumber(mem, offset, n1, 2);
  int n2 = num & 0xff;
  dumpNumber(mem, offset, n2, 8);
}

int Jcc_Form::getFirstImmAddr()
{
  return getOpcodeSize();
}

int Jcc_Form::getS( char* memory, int* rs, double* fs, int& psw )
{
  return imm_->getVal();
}

Null_Form::Null_Form(char opcode):Instruction(opcode)
{

}

Null_Form::~Null_Form()
{

}

int Null_Form::getSize()
{
  return getOpcodeSize() + 2;
}

CString Null_Form::toString()
{
  return getMnemonic();
}

void Null_Form::dump( char* mem, int& offset )
{
  dumpOpcode(mem, offset);
  dumpNumber(mem, offset, 3, 2);
}

Operand::Operand()
{

}

Operand::~Operand()
{

}

AModeOperand::AModeOperand( Amode* amode, GPR* gpr, Immediate* imm ):
  amode_(amode), gpr_(gpr), imm_(imm)
{

}

AModeOperand::~AModeOperand()
{
  delete amode_;
  delete gpr_;
  delete imm_;
}

int AModeOperand::getSize()
{
  switch (amode_->getNum()) {
  case 0:
  case 1:
    return 6;
  case 2:
    return 18;
  case 3:
    return 18;
  default:
    assert(0);
  }
  return -1;
}

CString AModeOperand::toString()
{
  switch (amode_->getNum()) {
  case 0:
    return CString("r" ) + CString(gpr_->getNum());
  case 1:
    return CString("[r") + CString(gpr_->getNum()) + CString(']');
  case 2:
    return CString(imm_->getVal()) + CString("[r") + CString(gpr_->getNum()) + CString(']');
  case 3:
    return CString(imm_->getVal());
  default:
    assert(0);
  }
  return "";
}

void AModeOperand::dump( char* mem, int& offset )
{
  dumpNumber(mem, offset, amode_->getNum(), 2);
  switch (amode_->getNum()) {
  case 0:
  case 1:
    dumpNumber(mem, offset, gpr_->getNum(), 4);
    break;
  case 2:
    {
      int num = imm_->getVal();
      int n1 = (num & 0xF00) >> 8;
      dumpNumber(mem, offset, n1, 4);
      int n2 = num & 0xff;
      dumpNumber(mem, offset, n2, 8);
    }
    dumpNumber(mem, offset, gpr_->getNum(), 4);
    break;
  case 3:
    dumpNumber(mem, offset, imm_->getVal(), 16);
    break;
  default:
    assert(0);
  } 
}

FPROperand::FPROperand(FPR* fpr):
  fpr_(fpr)
{

}

FPROperand::~FPROperand()
{
  delete fpr_;
}

int FPROperand::getSize()
{
  if (fpr_->getNum() < 0xf) {
    return 4;
  } else {
    assert(fpr_->getNum() == 0xf);
    return 70;
  }
}

CString FPROperand::toString()
{
  int fprNum = fpr_->getNum();
  if (fprNum < 0xf) {
    return CString('f') + CString(fprNum);
  } else {
    assert (fprNum == 0xf);
    return CString(fpr_->getImm());
  }
}

void FPROperand::dump( char* mem, int& offset )
{
  dumpNumber(mem, offset, fpr_->getNum(), 4);
  if (fpr_->getNum() == 0xf) {
    dumpNumber(mem, offset, 0, 2);
    double fimm = fpr_->getImm();
    int* iarr = (int*)&fimm;
    dumpNumber(mem, offset, iarr[1], 32);
    dumpNumber(mem, offset, iarr[0], 32);
  }
}

Amode::Amode( int num ):
  num_(num)
{

}

Amode::~Amode()
{

}

GPR::GPR( int num ):
  num_(num)
{

}

GPR::~GPR()
{

}

Immediate::Immediate( int val ):
  val_(val)
{

}

Immediate::~Immediate()
{

}

FPR::FPR( int num, double fimm):
  num_(num), fimm_(fimm)
{

}

FPR::~FPR()
{

}

Mov::Mov(AModeOperand* op1, AModeOperand* op2):D_S_Form(0, op1, op2)
{

}

Mov::~Mov()
{

}

void Mov::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest = src;
}

Add::Add(AModeOperand* op1, AModeOperand* op2):D_S_Form(1, op1, op2)
{

}

Add::~Add()
{

}

void Add::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest += src;
}

Sub::Sub(AModeOperand* op1, AModeOperand* op2):D_S_Form(2, op1, op2)
{

}

Sub::~Sub()
{

}

void Sub::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest -= src;
}

And::And(AModeOperand* op1, AModeOperand* op2):D_S_Form(3, op1, op2)
{

}

And::~And()
{

}

void And::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest &= src;
}

Or::Or(AModeOperand* op1, AModeOperand* op2):D_S_Form(4, op1, op2)
{

}

Or::~Or()
{

}

void Or::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest |= src;
}

Not::Not(AModeOperand* op1, AModeOperand* op2):D_S_Form(5, op1, op2)
{

}

Not::~Not()
{

}

void Not::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest = !src;
}

Xor::Xor(AModeOperand* op1, AModeOperand* op2):D_S_Form(6, op1, op2)
{

}

Xor::~Xor()
{

}

void Xor::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest ^= src;
}

Asl::Asl(AModeOperand* op1, AModeOperand* op2):D_S_Form(7, op1, op2)
{

}

Asl::~Asl()
{

}

void Asl::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest <<= src;
}

Asr::Asr(AModeOperand* op1, AModeOperand* op2):D_S_Form(8, op1, op2)
{

}

Asr::~Asr()
{

}

void Asr::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  *dest >>= src;
}

Push::Push(AModeOperand* op):D_Form(9, op)
{

}

Push::~Push()
{

}

void Push::emit( char* memory, int* rs, double* fs, int& psw )
{
  int src = getS(memory, rs, fs, psw);
  memory[rs[14]] = src;
  rs[14] -= 4;
}

Pop::Pop(AModeOperand* op):D_Form(0xa, op)
{

}

Pop::~Pop()
{

}

void Pop::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  rs[14] += 4;
  *dest = memory[rs[14]];
}

Lpsw::Lpsw(AModeOperand* op):D_Form(0xb, op)
{

}

Lpsw::~Lpsw()
{

}

void Lpsw::emit( char* memory, int* rs, double* fs, int& psw )
{
  int* dest = getD(memory, rs, fs, psw);
  *dest = psw;
}

Spsw::Spsw(AModeOperand* op):D_Form(0xc, op)
{

}

Spsw::~Spsw()
{

}

void Spsw::emit( char* memory, int* rs, double* fs, int& psw )
{
  int src = getS(memory, rs, fs, psw);
  psw = src;
}

Cmp::Cmp(AModeOperand* op1, AModeOperand* op2):D_S_Form(0x10, op1, op2)
{

}

Cmp::~Cmp()
{

}

void Cmp::emit( char* memory, int* rs, double* fs, int& psw )
{
  int dest = *getD(memory, rs, fs, psw);
  int src = getS(memory, rs, fs, psw);
  if (dest >= src) {
    psw |= 0x2;
  } else {
    psw &= 0xd;
  }
  if (dest != src) {
    psw |= 0x8;
  } else {
    psw &= 0x7;
  }
}

Jmp::Jmp(AModeOperand* op):D_Form(0x11, op)
{

}

Jmp::~Jmp()
{

}

void Jmp::emit( char* memory, int* rs, double* fs, int& psw )
{
  int src = getS(memory, rs, fs, psw);
  rs[15] = src;
}

Je::Je(Immediate* imm):Jcc_Form(0x12, imm)
{

}

Je::~Je()
{

}

void Je::emit( char* memory, int* rs, double* fs, int& psw )
{
  int Z = psw & 0x8;
  if (Z) {
    rs[15] += getS(memory, rs, fs, psw);
  }
}

Jne::Jne(Immediate* imm):Jcc_Form(0x13, imm)
{

}

Jne::~Jne()
{

}

void Jne::emit( char* memory, int* rs, double* fs, int& psw )
{
  int Z = psw & 0x8;
  if (!Z) {
    rs[15] += getS(memory, rs, fs, psw);
  }
}

Jg::Jg(Immediate* imm):Jcc_Form(0x14, imm)
{

}

Jg::~Jg()
{

}

void Jg::emit( char* memory, int* rs, double* fs, int& psw )
{
  int S = psw & 0x2;
  int Z = psw & 0x8;
  if (!Z && !S) {
    rs[15] += getS(memory, rs, fs, psw);
  }
}

Jge::Jge(Immediate* imm):Jcc_Form(0x15, imm)
{

}

Jge::~Jge()
{

}

void Jge::emit( char* memory, int* rs, double* fs, int& psw )
{
  int S = psw & 0x2;
  if (!S) {
    rs[15] += getS(memory, rs, fs, psw);
  }
}

Call::Call(AModeOperand* op):D_Form(0x16, op)
{

}

Call::~Call()
{

}

void Call::emit( char* memory, int* rs, double* fs, int& psw )
{
  memory[rs[14]] = rs[15];
  rs[14] -= 4;
  rs[15] = getS(memory, rs, fs, psw);
}

Ret::Ret():Null_Form(0x17)
{

}

Ret::~Ret()
{

}

void Ret::emit( char* memory, int* rs, double* fs, int& psw )
{
  rs[14] += 4;
  rs[15] = memory[rs[14]];
}

Fmov::Fmov(FPROperand* op1, FPROperand* op2):F_F_Form(0x20, op1, op2)
{

}

Fmov::~Fmov()
{

}

void Fmov::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  *dest = src;
}

Fadd::Fadd(FPROperand* op1, FPROperand* op2):F_F_Form(0x21, op1, op2)
{

}

Fadd::~Fadd()
{

}

void Fadd::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  *dest += src;
}

Fsub::Fsub(FPROperand* op1, FPROperand* op2):F_F_Form(0x22, op1, op2)
{

}

Fsub::~Fsub()
{

}

void Fsub::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  *dest -= src;
}

Fmul::Fmul(FPROperand* op1, FPROperand* op2):F_F_Form(0x23, op1, op2)
{

}

Fmul::~Fmul()
{

}

void Fmul::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  *dest *= src;
}

Fdiv::Fdiv(FPROperand* op1, FPROperand* op2):F_F_Form(0x24, op1, op2)
{

}

Fdiv::~Fdiv()
{

}

void Fdiv::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  *dest /= src;
}

Fcmp::Fcmp(FPROperand* op1, FPROperand* op2):F_F_Form(0x27, op1, op2)
{

}

Fcmp::~Fcmp()
{

}

void Fcmp::emit( char* memory, int* rs, double* fs, int& psw )
{
  double dest = *getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  if (dest < src) {
    psw |= 0x2; //S = 1
  } else {
    psw &= 0xd; //S = 0
  }
  if (dest == src) {
    psw |= 0x8; //Z = 1
  } else {
    psw &= 0x7; //Z = 0
  }
}

Fload::Fload(FPROperand* op1, AModeOperand* op2):F_G_Form(0x25, op1, op2)
{

}

Fload::~Fload()
{

}

void Fload::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  *dest = src;
}

Fstore::Fstore(AModeOperand* op1, FPROperand* op2):G_F_Form(0x26, op1, op2)
{

}

Fstore::~Fstore()
{

}

void Fstore::emit( char* memory, int* rs, double* fs, int& psw )
{
  double* dest = getFD(memory, rs, fs, psw);
  double src = getFS(memory, rs, fs, psw);
  if (dest_->getAmode()->getNum() == 0) {
    int* idest = (int*)dest;
    *idest = (int)src;
  } else {
    *dest = src;
  }
}
