#pragma once
#include "DynamicBody.h"
#include "ModelRenderable.h"
#include "Dockable.h"
//#include "StationDisplay.h"

class Ship;

class Station: public DynamicBody, public ModelRenderable, public Dockable
{
public:
  Station();
  ~Station();
  void render();
  void dock(Ship* ship);
  void undock(Ship* ship);
  //void setDisplay(StationDisplay* display) {display_ = display;}
  void setDockingLightsOn(bool value) {dockingLightsOn_ = value;}
  bool getDockingLightsOn() {return dockingLightsOn_;}
protected:
  list<Ship*> dockedShips_;
  bool dockingLightsOn_;
  //StationDisplay* display_;
private:
};
