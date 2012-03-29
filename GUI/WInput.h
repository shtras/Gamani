#pragma once
#include "Widget.h"

class WInput: public Widget
{
public:
  WInput();
  virtual ~WInput();
  void click();
  void render(double left, double top, double width, double height);
  bool isInteractive() {return true;}
  bool grabsFocus() {return true;}
  void getFocus();
  void loseFocus();
  bool handlePressedKey(int key);
private:
  CString content_;
  bool focus_;
};
