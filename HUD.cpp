#include "StdAfx.h"
#include "HUD.h"
#include "NavDisplay.h"
#include "Ship.h"
#include "APDisplay.h"

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

void HUD::init(Ship* ship)
{
  NavDisplay* navDisplay = new NavDisplay();
  navDisplay->init();
  displays_.push_back(navDisplay);
  AddLayout.emit(navDisplay);

  NavDisplay* navDisplay1 = new NavDisplay();
  navDisplay1->init();
  navDisplay1->setDimensions(0.6, 0.4, 0.4, 0.4);
  navDisplay1->setRightAlign(true);
  displays_.push_back(navDisplay1);
  AddLayout.emit(navDisplay1);

  APDisplay* apDisp = new APDisplay(navDisplay, navDisplay1);
  apDisp->init();
  apDisp->setDimensions(0.3, 0.35, 0.4, 0.35);
  displays_.push_back(apDisp);
  AddLayout.emit(apDisp);

  //Has to be after all display initializations
  ship->setHUD(this);
  apDisp->setAP(ship->getAP());
}
