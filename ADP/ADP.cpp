#include "StdAfx.h"
#include "ADP.h"

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
  for (int i=0; i<16; ++i) {
    gpr_[i] = 0;
  }
  gpr_[14] = 0x100000 - 1;
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
  cout << instr->toString() << endl;
  gpr_[15] += instr->getSize()/8;
  instr->emit(memory_, gpr_, fpr_, psw_);
  for (int i=0; i<16; ++i) {
    cout << gpr_[i] << " ";
  }
  cout << endl;
  return true;
}
