#pragma once

class ADPCompiler
{
public:
  ADPCompiler();
  ~ADPCompiler();
  bool compile(CString fileName, char* memory, int size);
  int getLastAddr() {return lastAddr_;}
private:
  bool parseLine(CString line);
  bool createInstr(CString& label, CString& opcode, CString& op1, CString& op2);
  bool isLabel(CString op);
  bool fixLabels();
  vector<CString> format(CString line);
  ifstream file_;
  char* memory_;
  int size_;
  int addr_;
  int lastAddr_;
  map<CString, int> labels_;
  map<int, CString> labelRefs_;
  map<int, CString> relativeLabelRefs_;
};
