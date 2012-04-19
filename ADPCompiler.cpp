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

  return res;
}

bool ADPCompiler::createInstr(CString& label, CString& mnemonic, CString& op1, CString& op2)
{
  if (!label.isEmpty()) {
    labels_[label] = addr_;
  }
  Instruction* instr = ADPFactory::getInstance().createInstr(mnemonic, op1, op2);
  if (!instr) {
    return false;
  }

  return true;
}

bool ADPCompiler::parseLine(CString line)
{
  if (line.isEmpty()) {
    return true;
  }
  vector<CString> parts = format(line);
  assert(parts.size() >= 2);
  CString label = "";
  CString mnemonic = "";
  CString op1 = "";
  CString op2 = "";
  uint32_t idx = 0;
  CString first = parts[idx];
  if (first[first.getSize()-1] == ':') {
    label = first.left(first.getSize()-1);
    idx++;
    assert(parts.size() >= 3);
  }
  mnemonic = parts[idx];
  op1 = parts[idx+1];
  if (parts.size() == idx+3) {
    op2 = parts[idx+2];
  }
  assert(parts.size() < idx+4);

  if (!createInstr(label, mnemonic, op1, op2)) {
    return false;
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

  file_.close();
  return true;
}
