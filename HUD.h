#pragma once
#include "CommDisplay.h"

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
  void toggleComm();
private:
  list<Display*> displays_;
  Ship* ship_;
  CommDisplay* commDisp_;
  bool commActive_;
  bool visible_;
};

