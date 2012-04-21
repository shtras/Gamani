#include "StdAfx.h"
#include "ADPCompiler.h"
#include "ADPFactory.h"
#include "ADPISA.h"

ADPCompiler::ADPCompiler():memory_(NULL)
{

}

ADPCompiler::~ADPCompiler()
{

}

vector<CString> ADPCompiler::format(CString line)
{
  //label: opcode op1, op2
  //label:opcode op1,op2
  //opcode op1, op2
  //opcode op1,op2
  vector<CString> res;
  line = line.toLower();
  line.replace('\t', ' ');

  CString curr = "";

  for (int i=0; i<line.getSize(); ++i) {
    char c = line[i];
    if (c == ' ' || c == ',' || c == ';') {
      if (curr.getSize() > 0) {
        res.push_back(curr);
        curr = "";
      }
      if (c == ';') {
        break;
      }
    } else {
      curr += c;
    }
    if (c == ':') {
      if (curr.getSize() > 0) {
        res.push_back(curr);
        curr = "";
      }
    }
  }
  if (!curr.isEmpty()) {
    res.push_back(curr);
  }

  return res;
}

bool ADPCompiler::isLabel(CString op)
{
  if (op[0] >= '0' && op[0] <= '9') {
    return false;
  }
  if (op[0] < 'a' || op[0] > 'z') {
    return false;
  }
  if (op[0] == 'r') {
    if (op.getSize() == 2 && op[1] >= '0' && op[1] <= '5') {
      return false;
    }
  }
  if (op[0] == 'f') {
    if (op.getSize() == 2 && op[1] >= '0' && op[1] <= '6') {
      return false;
    }
  }
  if (op == "rsp") {
    return false;
  }
  if (op == "rip") {
    return false;
  }
  return true;
}

bool ADPCompiler::createInstr(CString& label, CString& mnemonic, CString& op1, CString& op2)
{
  if (!label.isEmpty()) {
    if (isLabel(label)) {
      labels_[label] = addr_;
    } else {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Wrong label syntax: ") + label);
      return false;
    }
  }
  Instruction* instr = ADPFactory::getInstance().createInstr(mnemonic, op1, op2);
  if (!instr) {
    return false;
  }
  if (isLabel(op1)) {
    int immAddr = instr->getFirstImmAddr();
    if (immAddr == -1) {
      delete instr;
      return false;
    }
    if (instr->isJcc()) {
      relativeLabelRefs_[addr_ + immAddr] = op1;
    } else {
      labelRefs_[addr_ + immAddr] = op1;
    }
  }
  if (isLabel(op2)) {
    int immAddr = instr->getSecondImmAddr();
    if (immAddr == -1) {
      delete instr;
      return false;
    }
    if (instr->isJcc()) {
      relativeLabelRefs_[addr_ + immAddr] = op2;
    } else {
      labelRefs_[addr_ + immAddr] = op2;
    }
  }
  int testAddr = addr_ + instr->getSize();
  instr->dump(memory_, addr_);
  assert (addr_ == testAddr);
  lastAddr_ = addr_;
  assert (addr_ % 8 == 0);
  delete instr;
  return true;
}

bool ADPCompiler::parseLine(CString line)
{
  if (line.isEmpty()) {
    return true;
  }
  vector<CString> parts = format(line);
  if (parts.size() == 0) {
    return true;
  }
  CString label = "";
  CString mnemonic = "";
  CString op1 = "";
  CString op2 = "";
  uint32_t idx = 0;
  CString first = parts[idx];
  if (first[first.getSize()-1] == ':') {
    label = first.left(first.getSize()-1);
    idx++;
    assert(parts.size() > 1);
  }
  mnemonic = parts[idx];
  if (parts.size() >= idx+2) {
    op1 = parts[idx+1];
  }
  if (parts.size() == idx+3) {
    op2 = parts[idx+2];
  }
  assert(parts.size() < idx+4);

  if (!createInstr(label, mnemonic, op1, op2)) {
    return false;
  }

  return true;
}

bool ADPCompiler::fixLabels()
{
  for (auto itr = relativeLabelRefs_.begin(); itr != relativeLabelRefs_.end(); ++itr) {
    int addr = (*itr).first;
    CString label = (*itr).second;
    if (labels_.count(label) != 1) {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Reference to label that doesn't exist: ") + label);
      return false;
    }
    int labelAddr = labels_[label];
    int nextInstrAddr = addr + 10;
    assert(nextInstrAddr % 8 == 0);
    int offset = (labelAddr - nextInstrAddr)/8;
    if (abs(offset) >= 0x3FE) {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Relative label can't reach target: ") + label + CString(" ") + CString((int)offset));
      return false;
    }
    int n1 = (offset & 0x300) >> 8;
    dumpNumber(memory_, addr, n1, 2);
    int n2 = offset & 0xff;
    dumpNumber(memory_, addr, n2, 8);
  }

  for (auto itr = labelRefs_.begin(); itr != labelRefs_.end(); ++itr) {
    int addr = (*itr).first;
    CString label = (*itr).second;
    if (labels_.count(label) != 1) {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Reference to label that doesn't exist: ") + label);
      return false;
    }
    int labelAddr = labels_[label];
    int nextInstrAddr = addr + 24;
    nextInstrAddr -= nextInstrAddr%8;
    assert(nextInstrAddr % 8 == 0);
    dumpNumber(memory_, addr, labelAddr/8, 16);
  }
  return true;
}

bool ADPCompiler::compile(CString fileName, char* memory, int size)
{
  memory_ = memory;
  size_ = size;
  addr_ = 0;
  file_.open(fileName);
  if (file_.fail()) {
    file_.clear(ios::failbit);
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Failed to open assembler file: ") + fileName);
    return false;
  }
  Logger::getInstance().log(INFO_LOG_NAME, CString("Starting compilation of ") + fileName);
  char buf[1024];
  while (!file_.eof()) {
    file_.getline(buf, 1024);
    CString line((buf));
    if (!parseLine(line)) {
      file_.close();
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Syntax error: ") + line);
      return false;
    }
  }

  if (!fixLabels()) {
    file_.close();
    return false;
  }

  file_.close();
  return true;
}
