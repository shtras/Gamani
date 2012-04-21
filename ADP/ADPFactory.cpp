#include "StdAfx.h"
#include "ADPFactory.h"

ADPFactory::ADPFactory()
{

}

ADPFactory::~ADPFactory()
{

}

double getDouble(CString& str)
{
  str = str.toLower();
  double res = 0;
  bool neg = false;
  int exp = 0;
  bool negativeExp = false;
  int decimalCnt = 1;
  bool afterDecimal = false;
  bool afterExp = false;
  for (int i=0; i<str.getSize(); ++i) {
    char c = str[i];
    int currdigit = 0;
    if (c == '+') {
      continue;
    } else if (i == 0 && c == '-') {
      neg = true;
      continue;
    } else if (c == '.') {
      res /= 10;
      afterDecimal = true;
      continue;
    } else if (c == 'e') {
      if (str[i+1] == '-') {
        ++i;
        negativeExp = true;
      }
      if (!afterDecimal) {
        res /= 10;
      }
      afterExp = true;
    } else if (c >= '0' && i <= '9') {
      currdigit = c - '0';
    } else {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Wrong double value: ") + str);
      return 0;
    }
    if (afterExp) {
      exp += currdigit;
      exp *= 10;
      continue;
    }
    if (afterDecimal) {
      res += currdigit * pow(10.0, -decimalCnt);
      ++decimalCnt;
      continue;
    }
    res += currdigit;
    res *= 10;
  }
  if (!afterDecimal && !afterExp) {
    res /= 10;
  }
  if (negativeExp) {
    exp = -exp;
  }
  exp /= 10;
  res *= pow(10.0, exp);
  if (neg) {
    res = -res;
  }
  return res;
}

int getHexInt(CString& op)
{
  int res = 0;
  for (int i=0; i<op.getSize(); ++i) {
    char c = op[i];
    if (c >= '0' && c <= '9') {
      res += (c-'0');
    } else {
      assert (c >= 'a' && c <= 'f');
      res += (c-'a'+10);
    }
    if (i != op.getSize() - 1) {
      res *= 16;
    }
  }
  return res;
}

int getDecInt(CString& op)
{
  int res = 0;
  bool neg = false;
  for (int i=0; i<op.getSize(); ++i) {
    char c = op[i];
    if (i == 0 && c == '-') {
      neg = true;
    }
    assert (c >= '0' && c <= '9');
    int v = c - '0';
    res += v;
    if (i != op.getSize() - 1) {
      res *= 10;
    }
  }
  if (neg) {
    res = -res;
  }
  return res;
}

int getInt(CString& op, bool& res)
{
  if (op[0] == '0' && op[1] == 'x') {
    res = true;
    return getHexInt(op.substr(2, op.getSize()-1));
  } else if ((op[0] >= '0' && op[0] <= '9') || op[0] == '-') {
    res = true;
    return getDecInt(op);
  } else {
    //Assuming label
    res = false;
    return 0;
  }
}

AModeOperand* ADPFactory::createAmodeOperand(CString& op)
{
  int amodeNum = -1;
  int gprNum = -1;
  int immVal = 0;
  Immediate* imm = NULL;

  if (op[0] == 'r') {
    amodeNum = 0;
    gprNum = op[1] - '0';
  } else if (op[0] == '[') {
    amodeNum = 1;
    gprNum = op[1] - '0';
  } else if (op[op.getSize()-1] == ']') {
    int idx = op.getIndex('[');
    bool number = false;
    immVal = getInt(op.substr(0, idx-1), number);
    imm = new Immediate(immVal);
    amodeNum = 2;
    gprNum = op[op.getSize()-2] - '0';
  } else {
    bool number = false;
    immVal = getInt(op, number);
    imm = new Immediate(immVal);
    amodeNum = 3;
  }
  GPR* gpr = NULL;
  if (gprNum != -1) {
    gpr = new GPR(gprNum);
  }
  assert(amodeNum != -1);
  Amode* amode = new Amode(amodeNum);
  return new AModeOperand(amode, gpr, imm);
}

FPROperand* ADPFactory::createFPROperand(CString& op)
{
  int val;
  double fimm = 0;
  if (op[0] == 'f') {
    val = op[1] - '0';
  } else {
    val = 0xf;
    fimm = getDouble(op);
  }
  FPR* fpr = new FPR(val, fimm);
  return new FPROperand(fpr);
}

Instruction* ADPFactory::createInstr(CString& mnemonic, CString& op1, CString& op2)
{
  bool shifted = false;
  if (mnemonic[mnemonic.getSize()-1] == 's') {
    shifted = true;
    mnemonic = mnemonic.substr(0, mnemonic.getSize()-2);
  }
  Instruction* res = NULL;
  bool number = false;
  if (Mov::isMe(mnemonic)) {
    res = new Mov(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Add::isMe(mnemonic)) {
    res = new Add(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Sub::isMe(mnemonic)) {
    res = new Sub(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (And::isMe(mnemonic)) {
    res = new And(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Or::isMe(mnemonic)) {
    res = new Or(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Not::isMe(mnemonic)) {
    res = new Not(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Xor::isMe(mnemonic)) {
    res = new Xor(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Asl::isMe(mnemonic)) {
    res = new Asl(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Asr::isMe(mnemonic)) {
    res = new Asr(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Push::isMe(mnemonic)) {
    res = new Push(createAmodeOperand(op1));
  } else if (Pop::isMe(mnemonic)) {
    res = new Pop(createAmodeOperand(op1));
  } else if (Lpsw::isMe(mnemonic)) {
    res = new Lpsw(createAmodeOperand(op1));
  } else if (Spsw::isMe(mnemonic)) {
    res = new Spsw(createAmodeOperand(op1));
  } else if (Cmp::isMe(mnemonic)) {
    res = new Cmp(createAmodeOperand(op1), createAmodeOperand(op2));
  } else if (Jmp::isMe(mnemonic)) {
    res = new Jmp(createAmodeOperand(op1));
  } else if (Je::isMe(mnemonic)) {
    res = new Je(new Immediate(getInt(op1, number)));
  } else if (Jne::isMe(mnemonic)) {
    res = new Jne(new Immediate(getInt(op1, number)));
  } else if (Jg::isMe(mnemonic)) {
    res = new Jg(new Immediate(getInt(op1, number)));
  } else if (Jge::isMe(mnemonic)) {
    res = new Jge(new Immediate(getInt(op1, number)));
  } else if (Call::isMe(mnemonic)) {
    res = new Call(createAmodeOperand(op1));
  } else if (Ret::isMe(mnemonic)) {
    res = new Ret();
  } else if (Fmov::isMe(mnemonic)) {
    res = new Fmov(createFPROperand(op1), createFPROperand(op2));
  } else if (Fadd::isMe(mnemonic)) {
    res = new Fadd(createFPROperand(op1), createFPROperand(op2));
  } else if (Fsub::isMe(mnemonic)) {
    res = new Fsub(createFPROperand(op1), createFPROperand(op2));
  } else if (Fmul::isMe(mnemonic)) {
    res = new Fmul(createFPROperand(op1), createFPROperand(op2));
  } else if (Fdiv::isMe(mnemonic)) {
    res = new Fdiv(createFPROperand(op1), createFPROperand(op2));
  } else if (Fload::isMe(mnemonic)) {
    res = new Fload(createFPROperand(op1), createAmodeOperand(op2));
  } else if (Fstore::isMe(mnemonic)) {
    res = new Fstore(createAmodeOperand(op1), createFPROperand(op2));
  } else if (Fcmp::isMe(mnemonic)) {
    res = new Fcmp(createFPROperand(op1), createFPROperand(op2));
  } else {

  }
  res->setShiftedSrc(shifted);
  return res;
}

FPROperand* ADPFactory::createFPROperand( char* mem, int& offset )
{
  int fprNum = getBits(mem, offset, 4);
  double immVal = 0;
  if (fprNum == 0xf) {
    int reserve = getBits(mem, offset, 2);
    assert(reserve == 0);
    int imm1 = get32bit(mem, offset);
    int imm2 = get32bit(mem, offset);
    int* iarr = (int*)&immVal;
    iarr[0] = imm2;
    iarr[1] = imm1;
  }
  FPR* fpr = new FPR(fprNum, immVal);
  return new FPROperand(fpr);
}

AModeOperand* ADPFactory::createAmodeOperand(char* mem, int& offset)
{
  int amodeNum = getBits(mem, offset, 2);
  int regNum = -1;
  int immVal = 0;
  bool hasImm = false;
  if (amodeNum == 0 || amodeNum == 1) {
    regNum = getBits(mem, offset, 4);
  } else if (amodeNum == 2) {
    hasImm = true;
    immVal = getBits(mem, offset, 12);
    regNum = getBits(mem, offset, 4);
  } else if (amodeNum == 3) {
    hasImm = true;
    int imm1 = getBits(mem, offset, 8);
    int imm2 = getBits(mem, offset, 8);
    immVal = (imm2 << 8) + imm1;
  } else {
    return NULL;
  }
  Immediate* imm = NULL;
  GPR* gpr = NULL;
  if (hasImm) {
    imm = new Immediate(immVal);
  }
  if (regNum != -1) {
    gpr = new GPR(regNum);
  }
  Amode* amode = new Amode(amodeNum);
  return new AModeOperand(amode, gpr, imm);
}

Instruction* ADPFactory::createD_S_Form(char opcode, char* mem, int& offset)
{
  AModeOperand* amodeOp1 = createAmodeOperand(mem, offset);
  int shiftedBit = getBits(mem, offset, 1);
  AModeOperand* amodeOp2 = createAmodeOperand(mem, offset);

  int zeros = (8 - offset%8);
  if (zeros != 8) {
    int reserve = getBits(mem, offset, zeros);
    assert(reserve == 0);
  }

  Instruction* res = NULL;
  switch (opcode) {
  case 0x0:
    res = new Mov(amodeOp1, amodeOp2);
    break;
  case 0x1:
    res = new Add(amodeOp1, amodeOp2);
    break;
  case 0x2:
    res = new Sub(amodeOp1, amodeOp2);
    break;
  case 0x3:
    res = new And(amodeOp1, amodeOp2);
    break;
  case 0x4:
    res = new Or(amodeOp1, amodeOp2);
    break;
  case 0x5:
    res = new Not(amodeOp1, amodeOp2);
    break;
  case 0x6:
    res = new Xor(amodeOp1, amodeOp2);
    break;
  case 0x7:
    res = new Asl(amodeOp1, amodeOp2);
    break;
  case 0x8:
    res = new Asr(amodeOp1, amodeOp2);
    break;
  case 0x10:
    res = new Cmp(amodeOp1, amodeOp2);
    break;
  }
  res->setShiftedSrc(shiftedBit == 1);
  return res;
}

Instruction* ADPFactory::createD_Form( char opcode, char* mem, int& offset )
{
  AModeOperand* op = NULL;
  op = createAmodeOperand(mem, offset);

  int zeros = (8 - offset%8);
  if (zeros != 8) {
    int reserve = getBits(mem, offset, zeros);
    assert(reserve == 0);
  }
  Instruction* res = NULL;
  switch (opcode) {
  case 0x9:
    res = new Push(op);
    break;
  case 0xa:
    res = new Pop(op);
    break;
  case 0xb:
    res = new Lpsw(op);
    break;
  case 0xc:
    res = new Spsw(op);
    break;
  case 0x11:
    res = new Jmp(op);
    break;
  case 0x16:
    res = new Call(op);
    break;
  }
  return res;
}

Instruction* ADPFactory::createF_F_Form( char opcode, char* mem, int& offset )
{
  FPROperand* fprOp1 = createFPROperand(mem, offset);
  FPROperand* fprOp2 = createFPROperand(mem, offset);
  int zeros = (8 - offset%8);
  if (zeros != 8) {
    int reserve = getBits(mem, offset, zeros);
    assert(reserve == 0);
  }
  switch (opcode) {
  case 0x20:
    return new Fmov(fprOp1, fprOp2);
  case 0x21:
    return new Fadd(fprOp1, fprOp2);
  case 0x22:
    return new Fsub(fprOp1, fprOp2);
  case 0x23:
    return new Fmul(fprOp1, fprOp2);
  case 0x24:
    return new Fdiv(fprOp1, fprOp2);
  case 0x27:
    return new Fcmp(fprOp1, fprOp2);
  }
  return NULL;
}

Instruction* ADPFactory::createJcc_Form( char opcode, char* mem, int& offset )
{
  int imm1 = getBits(mem, offset, 10);
  //int imm2 = getBits(mem, offset, 8);
  int immVal = imm1;//(imm2 << 2) + imm1;
  if ((immVal & 0x200) != 0) {
    //sign extend
    immVal |= 0xFFFFFC00;
  }
  Immediate* imm = new Immediate(immVal);
  switch (opcode) {
  case 0x12:
    return new Je(imm);
  case 0x13:
    return new Jne(imm);
  case 0x14:
    return new Jg(imm);
  case 0x15:
    return new Jge(imm);
  }
  return NULL;
}

Instruction* ADPFactory::createNull_Form( char opcode, char* mem, int& offset )
{
  int reserve = getBits(mem, offset, 2);
  assert(reserve == 3);
  switch (opcode) {
  case 0x17:
    return new Ret();
  }
  return NULL;
}

Instruction* ADPFactory::createG_F_Form( char opcode, char* mem, int& offset )
{
  AModeOperand* op1 = createAmodeOperand(mem, offset);
  FPROperand* op2 = createFPROperand(mem, offset);
  switch (opcode) {
  case 0x26:
    return new Fstore(op1, op2);
  }
  return NULL;
}

Instruction* ADPFactory::createF_G_Form( char opcode, char* mem, int& offset )
{
  FPROperand* op1 = createFPROperand(mem, offset);
  AModeOperand* op2 = createAmodeOperand(mem, offset);
  switch (opcode) {
  case 0x25:
    return new Fload(op1, op2);
  }
  return NULL;
}

Instruction* ADPFactory::createInstr(char* mem, int& offset)
{
  char opcode = getBits(mem, offset, 6);
  Instruction* res = NULL;
  switch (opcode) {
  case 0x0:
  case 0x1:
  case 0x2:
  case 0x3:
  case 0x4:
  case 0x5:
  case 0x6:
  case 0x7:
  case 0x8:
  case 0x10:
    res = createD_S_Form(opcode, mem, offset);
    break;
  case 0x9:
  case 0xa:
  case 0xb:
  case 0xc:
  case 0x11:
  case 0x16:
    res =  createD_Form(opcode, mem, offset);
    break;
  case 0x20:
  case 0x21:
  case 0x22:
  case 0x23:
  case 0x24:
  case 0x27:
    res = createF_F_Form(opcode, mem, offset);
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
    res = createJcc_Form(opcode, mem, offset);
    break;
  case 0x17:
    res = createNull_Form(opcode, mem, offset);
    break;
  case 0x25:
    res = createF_G_Form(opcode, mem, offset);
    break;
  case 0x26:
    res = createG_F_Form(opcode, mem, offset);
    break;
  }
  int zeros = 8 - offset%8;
  if (zeros != 8) {
    int reserve = getBits(mem, offset, zeros);
    assert (reserve == 0);
  }
  return res;
}
