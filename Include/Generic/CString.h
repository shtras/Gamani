#pragma once
#include <string>
using namespace std;

class CString
{
public:
  CString();
  CString(const CString& other);
  CString(const char* str);
  CString(char c);
  CString(int val);
  CString(float val);
  CString(double val);
  CString(double val, int len);
  CString(long val);
  ~CString();

  CString left(int idx);
  int getSize() const;
  CString append(const CString& other) const;
  int getIndex(char c) const;
  bool contains(CString& other) const;
  operator const char*() const;
  CString operator+ (const CString& other) const;
  CString& operator= (CString& other);
  CString& operator= (const char* str);
  bool operator== (const CString& other) const;
  bool operator< (const CString& other) const;
private:
  char* cont_;
  int len_;

  //string str_;
};
