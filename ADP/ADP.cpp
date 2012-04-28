#include "StdAfx.h"
#include "ADP.h"
#include "Gamani.h"

ADP::ADP():memory_(new char[0x100000]),running_(false)
{

}

ADP::~ADP()
{
  delete[] memory_;
}

void ADP::reset()
{
  delete[] memory_;
  memory_ = new char[0x100000];
  /*
  0x9000 - sensor irq addr
  0x9500 - 0x9507 - yawVel
  */
  memory_[0x9000] = -1;
  memory_[0x10000] = 0;
  memory_[0x10001] = 0;
  memory_[0x10002] = 0;
  memory_[0x10003] = 0;
  memory_[0x10004] = 0;
  memory_[0x10005] = 0;
  memory_[0x10006] = 0;
  memory_[0x10007] = 0;
  memory_[0x10008] = 0;
  for (int i=0; i<16; ++i) {
    gpr_[i] = 0;
  }
  gpr_[14] = 0x100000 - 4;
  for (int i=0; i<15; ++i) {
    fpr_[i] = 0;
  }
  psw_ = 0;
  for (auto itr = instrCache_.begin(); itr != instrCache_.end(); ++itr) {
    int addr = (*itr).first;
    Instruction* instr = (*itr).second;
    delete instr;
  }
  instrCache_.clear();
}

void ADP::setMem(int addr, double value)
{
  *(double*)(memory_+addr) = value;
}

double ADP::getMem(int addr)
{
  return *(double*)(memory_+addr);
}

void ADP::irq(int num)
{
  if (!running_) {
    return;
  }
  if ((psw_ & 0x1) != 0) {
    return;
  }
  if (*(int*)(memory_+0x9000) != -1) {
    psw_ |= 0x1;
    gpr_[14] -= 4;
    *(int*)(memory_+gpr_[14]) = gpr_[15];
    gpr_[15] = *(int*)(memory_+0x9000);
  }
}

bool ADP::loadProgram(char* prog, int size)
{
  reset();
  int offset = 0;
  int addr = 0;
  while(1) {
    Instruction* instr = ADPFactory::getInstance().createInstr(prog, offset);
    if (!instr) {
      return false;
    }
    instrCache_[addr] = instr;
    int instrSize = instr->getSize();
    assert (instrSize % 8 == 0);
    int byteSize = instrSize / 8;
    for (int i=0; i<byteSize; ++i) {
      memory_[i + addr] = prog[i + addr];
    }
    addr += byteSize;
    if (offset >= size*8) {
      break;
    }
  }
  running_ = true;
  return true;
}

bool ADP::step()
{
  int pc = gpr_[15];
  if (instrCache_.count(pc) == 0) {
    running_ = false;
    return false;
  }
  Instruction* instr = instrCache_[pc];
  gpr_[15] += instr->getCachedSize()/8;
  instr->emit(memory_, gpr_, fpr_, psw_);
  
  if (Gamani::getInstance().printASM()) {
    cout << hex << pc << " " << instr->toString() << endl;
    for (int i=0; i<16; ++i) {
      cout << i << ":" << gpr_[i] << " ";
    }
    for (int i=0; i<15; ++i) {
      cout << i << ":" << fpr_[i] << " ";
    }
    cout << endl;
  }

  return true;
}

void ADP::halt()
{
  running_ = false;
  reset();
}
