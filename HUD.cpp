#include "StdAfx.h"
#include "HUD.h"
#include "NavDisplay.h"
#include "Ship.h"
#include "APDisplay.h"

HUD::HUD():ship_(NULL),commActive_(false)
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
  //if (visible_) {
  //  if (ship_->isDocked() && !stationDisp_->isVisible()) {
  //    stationDisp_->setVisible(true);
  //  } else if (!ship_->isDocked() && stationDisp_->isVisible()) {
  //    stationDisp_->setVisible(false);
  //  }
  //}
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
  navDisplay->setBottom(true);
  displays_.push_back(navDisplay);
  AddLayout.emit(navDisplay);

  NavDisplay* navDisplay1 = new NavDisplay();
  navDisplay1->init();
  navDisplay1->setDimensions(0.6, 0.4, 0.4, 0.4);
  navDisplay1->setRightAlign(true);
  navDisplay1->setBottom(true);
  displays_.push_back(navDisplay1);
  AddLayout.emit(navDisplay1);

  APDisplay* apDisp = new APDisplay(navDisplay, navDisplay1);
  apDisp->init();
  apDisp->setDimensions(0.3, 0.35, 0.4, 0.35);
  apDisp->setBottom(true);
  displays_.push_back(apDisp);
  AddLayout.emit(apDisp);

  commDisp_ = new CommDisplay(navDisplay, navDisplay1);
  commDisp_->init();
  commDisp_->setDimensions(0.7, 0.9, 0.3, 0.45);
  displays_.push_back(commDisp_);
  AddLayout.emit(commDisp_);

  //Has to be after all display initializations
  ship->setHUD(this);
  apDisp->setAP(ship->getAP());
}

void HUD::toggleComm()
{
  commActive_ = !commActive_;
  if (commActive_) {
    commDisp_->setVisible(true);
  } else {
    commDisp_->setVisible(false);
  }
}

void HUD::setVisible(bool value)
{
  auto itr = displays_.begin();
  for (; itr != displays_.end(); ++itr) {
    Display* display = *itr;
    display->setVisible(value);
  }
  visible_ = value;
  if (!commActive_) {
    commDisp_->setVisible(false);
  }
}
