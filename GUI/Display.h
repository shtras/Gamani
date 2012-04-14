#pragma once
#include "WLayout.h"
#include "Ship.h"

class Display: public WLayout
{
public:
  Display();
  ~Display();
  void assignTo(Ship* ship);
  virtual void updateData() = 0;
protected:
  Ship* ship_;
};

