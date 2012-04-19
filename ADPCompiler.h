#pragma once

class ADPCompiler
{
public:
  ADPCompiler();
  ~ADPCompiler();
  bool compile(CString fileName, char* memory, int size);
private:
  bool parseLine(CString line);
  bool createInstr(CString& label, CString& opcode, CString& op1, CString& op2);
  vector<CString> format(CString line);
  ifstream file_;
  char* memory_;
  int size_;
  int addr_;
  map<CString, int> labels_;
  map<int, CString> labelRefs_;
};
