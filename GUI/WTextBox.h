#pragma once
#include "Widget.h"

#define LINE_HEIGHT (15)

class WTextBox: public Widget
{
public:
  WTextBox();
  ~WTextBox();
  void render(double left, double top, double width, double height);
  bool isInteractive() {return true;}
  virtual void setDimensions(double left, double top, double width, double height);
  void click() {}
  void addLine(CString line);
  void reset();
private:
  vector<CString> lines_;
  uint32_t topLineIdx_;
  uint32_t visibleLines_;
};
