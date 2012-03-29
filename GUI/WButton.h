#pragma once
#include "Widget.h"

class WButton: public Widget
{
public:
  WButton();
  ~WButton();
  void click();
  void render(double left, double top, double width, double height);
  bool isInteractive() {return true;}
  void setLabel(CString label) {label_ = label;}
  CString getLabel() {return  label_;}

  signal0<> sigClick;
private:
  CString label_;
};

