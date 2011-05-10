#include "StdAfx.h"
#include "CString.h"
#include <sstream>

CString::CString()
{
  cont_ = (char*)malloc(1);
  cont_[0] = '\0';
  len_ = 0;
}

CString::CString(const CString& other)
{
  cont_ = (char*)malloc(other.len_+1);
  strcpy(cont_, other.cont_);
  len_ = other.len_;
}

CString::CString(const char* str)
{
  len_ = strlen(str);
  cont_ = (char*)malloc(len_+1);
  strcpy(cont_, str);
}

CString::CString(int val)
{
  int origval = val;
  int len = (val > 0)? 0:1;
  while (val != 0) {
    val /= 10;
    len++;
  }
  cont_ = (char*)malloc(len);
  _itoa(origval, cont_, 10);
  len_ = len;
}

CString::CString(float val)
{

}

CString::CString(double val)
{
  std::stringstream ss;
  ss << val;
  std::string str = ss.str();
  len_ = str.size();
  cont_ = (char*)malloc(len_+1);
  strcpy(cont_, str.c_str());
}

CString::CString(long val)
{

}

CString::CString (char val)
{

}

CString::~CString()
{
  free(cont_);
  cont_ = NULL;
}

int CString::getSize() const
{
  return len_;
}

CString CString::append(const CString& other) const
{
  CString res;
  free(res.cont_);
  res.cont_ = (char*)malloc(len_ + other.len_ + 1);
  strcpy(res.cont_, cont_);
  strcpy(res.cont_ + len_, other.cont_);
  res.len_ = len_ + other.len_;
  return res;
}

int CString::getIndex(char c) const
{
  return -1;
}

bool CString::contains(CString& other) const
{
  return false;
}

CString::operator const char*() const
{
  return cont_;
}

CString CString::operator+ (const CString& other) const
{
  return this->append(other);
}

CString& CString::operator= (CString& other)
{
  free(cont_);
  cont_ = (char*)malloc(other.len_+1);
  strcpy(cont_, other.cont_);
  len_ = other.len_;
  return *this;
}

CString& CString::operator= (const char* str)
{
  free(cont_);
  len_ = strlen(str);
  cont_ = (char*)malloc(len_+1);
  strcpy(cont_, str);
  return *this;
}

bool CString::operator== (const CString& other) const
{
  if (other.len_ != len_) {
    return false;
  }
  return (strcmp(cont_, other.cont_) == 0);
}
