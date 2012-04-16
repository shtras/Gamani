#pragma once
#include "WLayout.h"
#include "WTextBox.h"

class TextPanel: public WLayout
{
public:
  TextPanel();
  ~TextPanel();
  virtual void init();
  void setText(const vector<CString>& text);
private:
  WTextBox* textBox_;
  void closeButtonClick();
};
