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
  void setParam(void* param);
  signal0<> sigClick;
  signal1<void*> sigClickParam;
  void setHighlighted(bool value) {highlighted_ = value;}
private:
  CString label_;
  void* param_;
  bool hasParam_;
  bool highlighted_;
};

