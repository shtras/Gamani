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
      res += (c-'a');
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
  for (int i=0; i<op.getSize(); ++i) {
    char c = op[i];
    assert (c >= '0' && c <= '9');
    int v = c - '0';
    res += v;
    if (i != op.getSize() - 1) {
      res *= 10;
    }
  }
  return res;
}

int getInt(CString& op)
{
  if (op[0] == '0' && op[1] == 'x') {
    return getHexInt(op);
  } else {
    return getDecInt(op);
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
    gprNum = op[2] - '0';
    if (op[op.getSize()-2] == '+') {
      assert(op[op.getSize()-3] == '+');
      amodeNum = 2;
    } else if (op[op.getSize()-2] == '-') {
      assert(op[op.getSize()-3] == '-');
      amodeNum = 3;
    } else {
      amodeNum = 1;
    }
  } else if (op[op.getSize()-1] == ']') {
    int idx = op.getIndex('[');
    immVal = getInt(op.substr(0, idx-1));
    imm = new Immediate(immVal);
    amodeNum = 6;
    gprNum = op[op.getSize()-3] - '0';
  } else {
    immVal = getInt(op);
    imm = new Immediate(immVal);
    if (immVal <= 0xf) {
      amodeNum = 5;
    } else {
      amodeNum = 7;
    }
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
    val = 7;
    fimm = getDouble(op);
  }
  FPR* fpr = new FPR(val, fimm);
  return new FPROperand(fpr);
}

Instruction* ADPFactory::createInstr(CString& mnemonic, CString& op1, CString& op2)
{
  return NULL;
}
