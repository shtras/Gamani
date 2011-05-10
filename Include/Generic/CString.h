#pragma once

class CString
{
public:
  CString();
  CString(const CString& other);
  CString(const char* str);
  CString(int val);
  CString(float val);
  CString(double val);
  CString(long val);
  CString (char val);
  ~CString();

  int getSize() const;
  CString append(const CString& other) const;
  int getIndex(char c) const;
  bool contains(CString& other) const;
  operator const char*() const;
  CString operator+ (const CString& other) const;
  CString& operator= (CString& other);
  CString& operator= (const char* str);
  bool operator== (const CString& other) const;
private:
  char* cont_;
  int len_;
};
