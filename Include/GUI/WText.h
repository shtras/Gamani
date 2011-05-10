#pragma once
#include "widget.h"
class WText: public Widget
{
public:
  WText();
  WText(CString text):text_(text) {}
  ~WText();
  void render(double left, double top, double width, double height);
  void setText(CString text) {text_ = text;}
  void click() {}
protected:
  CString text_;
};

