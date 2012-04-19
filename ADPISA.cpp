#include "StdAfx.h"
#include "ADPISA.h"

Instruction::Instruction( char opcode )
{

}

Instruction::~Instruction()
{

}

D_S_Form::D_S_Form(char opcode, AModeOperand* op1, AModeOperand* op2):Instruction(opcode), dest_(op1), src_(op2)
{

}

D_S_Form::~D_S_Form()
{

}

int D_S_Form::getSize()
{
  return 0;
}

D_Form::D_Form(char opcode):Instruction(opcode)
{

}

D_Form::~D_Form()
{

}

int D_Form::getSize()
{
  return 0;
}

F_F_Form::F_F_Form(char opcode):Instruction(opcode)
{

}

F_F_Form::~F_F_Form()
{

}

int F_F_Form::getSize()
{
  return 0;
}

G_F_Form::G_F_Form(char opcode):Instruction(opcode)
{

}

G_F_Form::~G_F_Form()
{

}

int G_F_Form::getSize()
{
  return 0;
}

F_G_Form::F_G_Form(char opcode):Instruction(opcode)
{

}

F_G_Form::~F_G_Form()
{

}

int F_G_Form::getSize()
{
  return 0;
}

Jcc_Form::Jcc_Form(char opcode):Instruction(opcode)
{

}

Jcc_Form::~Jcc_Form()
{

}

int Jcc_Form::getSize()
{
  return 0;
}

Null_Form::Null_Form(char opcode):Instruction(opcode)
{

}

Null_Form::~Null_Form()
{

}

int Null_Form::getSize()
{
  return 0;
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

}

int AModeOperand::getSize()
{
  return 0;
}

FPROperand::FPROperand(FPR* fpr):
  fpr_(fpr)
{

}

FPROperand::~FPROperand()
{

}

int FPROperand::getSize()
{
  return 0;
}

Amode::Amode( int num ):
  num_(num)
{

}

Amode::~Amode()
{

}

int Amode::getSize()
{
  return 0;
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

int FPR::getSize()
{
  return 0;
}

Mov::Mov(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+0, op1, op2)
{

}

Mov::~Mov()
{

}

Add::Add(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+1, op1, op2)
{

}

Add::~Add()
{

}

Sub::Sub(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+2, op1, op2)
{

}

Sub::~Sub()
{

}

And::And(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+3, op1, op2)
{

}

And::~And()
{

}

Or::Or(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+4, op1, op2)
{

}

Or::~Or()
{

}

Not::Not(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+5, op1, op2)
{

}

Not::~Not()
{

}

Xor::Xor(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+6, op1, op2)
{

}

Xor::~Xor()
{

}

Asl::Asl(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+7, op1, op2)
{

}

Asl::~Asl()
{

}

Asr::Asr(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+8, op1, op2)
{

}

Asr::~Asr()
{

}

Push::Push():D_Form('0'+9)
{

}

Push::~Push()
{

}

Pop::Pop():D_Form('0'+0xa)
{

}

Pop::~Pop()
{

}

Lpsw::Lpsw():D_Form('0'+0xb)
{

}

Lpsw::~Lpsw()
{

}

Spsw::Spsw():D_Form('0'+0xc)
{

}

Spsw::~Spsw()
{

}

Cmp::Cmp(AModeOperand* op1, AModeOperand* op2):D_S_Form('0'+0x10, op1, op2)
{

}

Cmp::~Cmp()
{

}

Jmp::Jmp():D_Form('0'+0x11)
{

}

Jmp::~Jmp()
{

}

Je::Je():Jcc_Form('0'+0x12)
{

}

Je::~Je()
{

}

Jne::Jne():Jcc_Form('0'+0x13)
{

}

Jne::~Jne()
{

}

Jg::Jg():Jcc_Form('0'+0x14)
{

}

Jg::~Jg()
{

}

Jge::Jge():Jcc_Form('0'+0x15)
{

}

Jge::~Jge()
{

}

Call::Call():D_Form('0'+0x16)
{

}

Call::~Call()
{

}

Ret::Ret():Null_Form('0'+0x17)
{

}

Ret::~Ret()
{

}

Fmov::Fmov():F_F_Form('0'+0x20)
{

}

Fmov::~Fmov()
{

}

Fadd::Fadd():F_F_Form('0'+0x21)
{

}

Fadd::~Fadd()
{

}

Fsub::Fsub():F_F_Form('0'+0x22)
{

}

Fsub::~Fsub()
{

}

Fmul::Fmul():F_F_Form('0'+0x23)
{

}

Fmul::~Fmul()
{

}

Fdiv::Fdiv():F_F_Form('0'+0x24)
{

}

Fdiv::~Fdiv()
{

}

Fcmp::Fcmp():F_F_Form('0'+0x27)
{

}

Fcmp::~Fcmp()
{

}

FLoad::FLoad():F_G_Form('0'+0x25)
{

}

FLoad::~FLoad()
{

}

Fstore::Fstore():G_F_Form('0'+0x26)
{

}

Fstore::~Fstore()
{

}
