#include "StdAfx.h"
#include "HUD.h"
#include "NavDisplay.h"
#include "Ship.h"

HUD::HUD():ship_(NULL)
{
}

HUD::~HUD()
{
  list<Display*>::iterator itr = displays_.begin();
  for (; itr != displays_.end(); ++itr) {
    Display* display = *itr;
    RemoveLayout.emit(display);
    delete display;
  }
}

void HUD::updateData()
{
  list<Display*>::iterator itr = displays_.begin();
  for (; itr != displays_.end(); ++itr) {
    Display* display = *itr;
    display->updateData();
  }
}

void HUD::assignTo(Ship* ship)
{
  ship_ = ship;
  list<Display*>::iterator itr = displays_.begin();
  for (; itr != displays_.end(); ++itr) {
    Display* display = *itr;
    display->assignTo(ship);
  }
}

void HUD::init()
{
  NavDisplay* navDisplay = new NavDisplay();
  navDisplay->init();
  displays_.push_back(navDisplay);
  AddLayout.emit(navDisplay);
}
