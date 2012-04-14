#pragma once
#include "Display.h"

class StationDisplay: public Display
{
public:
  StationDisplay();
  ~StationDisplay();
  void render();
  void init();
  virtual void updateData();
private:
};
