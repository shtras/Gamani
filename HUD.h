#pragma once
#include "StationDisplay.h"

class Ship;

class HUD
{
public:
  HUD();
  ~HUD();
  void init(Ship* ship);
  void assignTo(Ship* ship);
  void updateData();

  signal1<WLayout*> AddLayout;
  signal1<WLayout*> RemoveLayout;
  void setVisible(bool value);
private:
  list<Display*> displays_;
  Ship* ship_;
  StationDisplay* stationDisp_;
  bool visible_;
};

