#include "StdAfx.h"
#include "Autopilot.h"
#include "Ship.h"
#include "APProgram.h"
#include "Gamani.h"

Autopilot::Autopilot(Ship* ship):ship_(ship),ref_(NULL),lastError_("No errors")
{

}

Autopilot::~Autopilot()
{
  clearQueue();
}

void Autopilot::clearQueue()
{
  GC();
  while (programs_.size() > 0) {
    APProgram* prog = programs_.back();
    delete prog;
    programs_.pop_back();
  }
  assert (programs_.size() == 0);
  assert (garbage_.size() == 0);
}

void Autopilot::step()
{
  if (programs_.size() == 0) {
    return;
  }
  {
    //list<APProgram*>::iterator itr = programs_.begin();
    //for (; itr != programs_.end(); ++itr) {
    //  APProgram* prog = *itr;
    //  cout << prog->getID() << " ";
    //}
    //cout << endl;
  }
  programs_.front()->init();
  programs_.front()->step();
  GC();
}

void Autopilot::GC()
{
  if (garbage_.size() == 0) {
    return;
  }
  while (garbage_.size() > 0) {
    APProgram* prog = garbage_.back();
    delete prog;
    garbage_.pop_back();
  }
}

void Autopilot::addProgram(APProgram* prog)
{
  programs_.push_back(prog);
  lastError_ = "No errors";
}

void Autopilot::addImmediateProgram(APProgram* prog)
{
  programs_.push_front(prog);
}

void Autopilot::endCurrProg()
{
  APProgram* prog = programs_.front();
  programs_.pop_front();
  garbage_.push_front(prog);
  if (programs_.size() == 0) {
    Gamani::getInstance().setSpeed1x();
  }
}

Autopilot::ProgID Autopilot::getCurrProg()
{
  if (programs_.size() == 0) {
    return NoProgram;
  }
  return programs_.front()->getID();
}

CString Autopilot::getProgInfo()
{
  if (programs_.size() == 0) {
    return "Idle";
  }
  return programs_.front()->getInfo();
}
