#include "StdAfx.h"
#include "ADPISA.h"

Instruction::Instruction( char opcode )
{

}

Instruction::~Instruction()
{

}

D_S_Form::D_S_Form(char opcode):Instruction(opcode)
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

FPROperand::FPROperand(FPR* fpr)
{

}

FPROperand::~FPROperand()
{

}

int FPROperand::getSize()
{
  return 0;
}

Amode::Amode( int num )
{

}

Amode::~Amode()
{

}

int Amode::getSize()
{
  return 0;
}

GPR::GPR( int num )
{

}

GPR::~GPR()
{

}

Immediate::Immediate( int val )
{

}

Immediate::~Immediate()
{

}

FPR::FPR( int num, double fimm)
{

}

FPR::~FPR()
{

}

int FPR::getSize()
{

}

Mov::Mov():D_S_Form('0'+0)
{

}

Mov::~Mov()
{

}

Add::Add():D_S_Form('0'+1)
{

}

Add::~Add()
{

}

Sub::Sub():D_S_Form('0'+2)
{

}

Sub::~Sub()
{

}

And::And():D_S_Form('0'+3)
{

}

And::~And()
{

}

Or::Or():D_S_Form('0'+4)
{

}

Or::~Or()
{

}

Not::Not():D_S_Form('0'+5)
{

}

Not::~Not()
{

}

Xor::Xor():D_S_Form('0'+6)
{

}

Xor::~Xor()
{

}

Asl::Asl():D_S_Form('0'+7)
{

}

Asl::~Asl()
{

}

Asr::Asr():D_S_Form('0'+8)
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

Cmp::Cmp():D_S_Form('0'+0x10)
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
