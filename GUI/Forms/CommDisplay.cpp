#include "StdAfx.h"
#include "CommDisplay.h"

CommDisplay::CommDisplay(NavDisplay* left, NavDisplay* right):
left_(left),right_(right),target_(NULL)
{

}

CommDisplay::~CommDisplay()
{

}

void CommDisplay::updateData()
{
  CString tName = (target_)?target_->getName():"NULL";
  targetName_->setText(CString("Target: ") + tName);

  if (target_ && target_->getType() == Renderable::StationType && !ship_->isDocked()) {
    requestDockingButton_->setVisible(true);
    Station* station = static_cast<Station*>(target_);
    bool dockingInProgress = station->getDockingLightsOn();
    if (dockingInProgress) {
      requestDockingButton_->setLabel("Cancel docking");
    } else {
      requestDockingButton_->setLabel("Request docking");
    }
  } else {
    requestDockingButton_->setVisible(false);
  }
}

void CommDisplay::init()
{
  targetName_ = new WText();
  targetName_->setDimensions(0.4, 0.9, 0.3, 0.1);
  targetName_->setText("Target: NULL");
  addWidget(targetName_);

  fromLeftButton_ = new WButton();
  fromLeftButton_->setDimensions(0.04,0.85,0.1,0.1);
  fromLeftButton_->setLabel("<");
  fromLeftButton_->sigClick.connect(this, &CommDisplay::fromLeftClick);
  addWidget(fromLeftButton_);

  fromRightButton_ = new WButton();
  fromRightButton_->setDimensions(0.86,0.85,0.1,0.1);
  fromRightButton_->setLabel(">");
  fromRightButton_->sigClick.connect(this, &CommDisplay::fromRightClick);
  addWidget(fromRightButton_);

  requestDockingButton_ = new WButton();
  requestDockingButton_->setDimensions(0.3, 0.5, 0.4, 0.2);
  requestDockingButton_->setLabel("Request docking");
  requestDockingButton_->sigClick.connect(this, &CommDisplay::requestDockingClick);
  requestDockingButton_->setVisible(false);
  addWidget(requestDockingButton_);
}

void CommDisplay::fromLeftClick()
{
  target_ = left_->getGravityRef();
  updateTarget();
}

void CommDisplay::fromRightClick()
{
  target_ = right_->getGravityRef();
  updateTarget();
}

void CommDisplay::requestDockingClick()
{
  assert (target_ && target_->getType() == Renderable::StationType);
  Station* station = static_cast<Station*>(target_);
  bool dockingInProgress = station->getDockingLightsOn();
  if (dockingInProgress) {
    station->setDockingLightsOn(false);
  } else {
    station->setDockingLightsOn(true);
  }
}

void CommDisplay::updateTarget()
{
  if (!target_) {
    return;
  }
  if (target_->getType() == Renderable::StationType) {
    requestDockingButton_->setVisible(true);
    Station* station = static_cast<Station*>(target_);
    bool dockingInProgress = station->getDockingLightsOn();
    if (dockingInProgress) {
      requestDockingButton_->setLabel("Cancel docking");
    } else {
      requestDockingButton_->setLabel("Request docking");
    }
  } else {
    requestDockingButton_->setVisible(false);
  }
}

void CommDisplay::render()
{
  WLayout::render();
}
