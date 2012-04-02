#include "StdAfx.h"
#include "APDisplay.h"

APDisplay::APDisplay(NavDisplay* left, NavDisplay* right):
  refText_(NULL),fromLeftRefButton_(NULL),fromRightRefButton_(NULL),progNameText_(NULL),statusText_(NULL),landedDockedText_(NULL),
  killRotProgButton_(NULL),rotateProgButton_(NULL),launchProgButton_(NULL),approachProgButton_(NULL),
  left_(left),right_(right)
{

}

APDisplay::~APDisplay()
{
}

void APDisplay::init()
{
  refText_ = new WText("NULL");
  refText_->setDimensions(0.3,0.9,0.1,0.5);
  addWidget(refText_);

  progNameText_ = new WText("Program");
  progNameText_->setDimensions(0.4,0.8,0.1,0.6);
  addWidget(progNameText_);

  statusText_ = new WText("Program");
  statusText_->setDimensions(0.05,0.7,0.1,0.9);
  addWidget(statusText_);

  landedDockedText_ = new WText("");
  landedDockedText_->setDimensions(0.05,0.2,0.1,0.9);
  addWidget(landedDockedText_);

  errorText_ = new WText("Error");
  errorText_->setDimensions(0.05,0.1,0.1,0.9);
  addWidget(errorText_);

  fromLeftRefButton_ = new WButton();
  fromLeftRefButton_->setDimensions(0.1,0.85,0.1,0.1);
  fromLeftRefButton_->setLabel("<");
  fromLeftRefButton_->sigClick.connect(this, &APDisplay::leftRefClick);
  addWidget(fromLeftRefButton_);

  fromRightRefButton_ = new WButton();
  fromRightRefButton_->setDimensions(0.8,0.85,0.1,0.1);
  fromRightRefButton_->setLabel(">");
  fromRightRefButton_->sigClick.connect(this, &APDisplay::rightRefClick);
  addWidget(fromRightRefButton_);

  killRotProgButton_ = new WButton();
  killRotProgButton_->setDimensions(0.1,0.55,0.2,0.1);
  killRotProgButton_->setLabel("KillRot");
  killRotProgButton_->sigClick.connect(this, &APDisplay::killRotClick);
  addWidget(killRotProgButton_);

  rotateProgButton_ = new WButton();
  rotateProgButton_->setDimensions(0.4,0.55,0.2,0.1);
  rotateProgButton_->setLabel("Rotate");
  rotateProgButton_->sigClick.connect(this, &APDisplay::rotateClick);
  addWidget(rotateProgButton_);

  launchProgButton_ = new WButton();
  launchProgButton_->setDimensions(0.7,0.55,0.2,0.1);
  launchProgButton_->setLabel("Launch");
  launchProgButton_->sigClick.connect(this, &APDisplay::launchClick);
  addWidget(launchProgButton_);

  approachProgButton_ = new WButton();
  approachProgButton_->setDimensions(0.1,0.4,0.2,0.1);
  approachProgButton_->setLabel("Approach");
  approachProgButton_->sigClick.connect(this, &APDisplay::approachClick);
  addWidget(approachProgButton_);

  proGradeButton_ = new WButton();
  proGradeButton_->setDimensions(0.4,0.4,0.2,0.1);
  proGradeButton_->setLabel("Pro Grade");
  proGradeButton_->sigClick.connect(this, &APDisplay::proGradeClick);
  addWidget(proGradeButton_);

  retroGradeButton_ = new WButton();
  retroGradeButton_->setDimensions(0.7,0.4,0.2,0.1);
  retroGradeButton_->setLabel("Retro Grade");
  retroGradeButton_->sigClick.connect(this, &APDisplay::retroGradeClick);
  addWidget(retroGradeButton_);

  matchSpeedButton_ = new WButton();
  matchSpeedButton_->setDimensions(0.1, 0.25, 0.2, 0.1);
  matchSpeedButton_->setLabel("Match speed");
  matchSpeedButton_->sigClick.connect(this, &APDisplay::matchSpeed);
  addWidget(matchSpeedButton_);

  testInput_ = new WInput();
  testInput_->setDimensions(0.25,0.25,0.5,0.1);
  testInput_->setVisible(false);
  addWidget(testInput_);

  visible_ = true;
}

void APDisplay::leftRefClick()
{
  myAP_->setGravityRef(left_->getGravityRef());
}

void APDisplay::rightRefClick()
{
  myAP_->setGravityRef(right_->getGravityRef());
}

void APDisplay::killRotClick()
{
  myAP_->getShip()->setAutopilotTo(Autopilot::KillRot);
}

void APDisplay::rotateClick()
{
  myAP_->getShip()->setAutopilotTo(Autopilot::Rotate);
}

void APDisplay::launchClick()
{
  if (myAP_->getShip()->isLanded()) {
    myAP_->getShip()->setAutopilotTo(Autopilot::Launch);
  } else if (myAP_->getShip()->isDocked()) {
    myAP_->getShip()->undock();
  } else {
    myAP_->setError("Not available while in flight");
  }
}

void APDisplay::approachClick()
{
  myAP_->getShip()->setAutopilotTo(Autopilot::Approach);
}

void APDisplay::proGradeClick()
{
  myAP_->getShip()->setAutopilotTo(Autopilot::ProGrade);
}

void APDisplay::retroGradeClick()
{
  myAP_->getShip()->setAutopilotTo(Autopilot::RetroGrade);
}

void APDisplay::matchSpeed()
{
  myAP_->getShip()->setAutopilotTo(Autopilot::EqSpeed);
}

void APDisplay::render()
{
  WLayout::render();
}

void APDisplay::updateData()
{
  AstralBody* myRef = myAP_->getGravityRef();
  if (!myRef) {
    refText_->setText("NULL");
  } else {
    refText_->setText(myRef->getName());
  }

  progNameText_->setText(myAP_->getShip()->getCurrProgName());

  statusText_->setText(myAP_->getProgInfo());

  errorText_->setText(myAP_->getError());

  if (myAP_->getShip()->isLanded()) {
    landedDockedText_->setText("Landed");
    launchProgButton_->setLabel("Launch");
  } else if (myAP_->getShip()->isDocked()) {
    landedDockedText_->setText("Docked");
    launchProgButton_->setLabel("Undock");
  } else {
    landedDockedText_->setText("Free flight");
    launchProgButton_->setLabel("N/A");
  }
  if (myAP_->getShip()->isUndocking()) {
    landedDockedText_->setText("Undocking...");
  }
}