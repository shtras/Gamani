#pragma once
#include "Display.h"

class Ship;

class HUD
{
public:
  HUD();
  ~HUD();
  void init();
  void assignTo(Ship* ship);
  void updateData();

  signal1<WLayout*> AddLayout;
  signal1<WLayout*> RemoveLayout;
private:
  list<Display*> displays_;
  Ship* ship_;
};

