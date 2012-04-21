#include "StdAfx.h"
#include "Autopilot.h"
#include "Ship.h"
#include "APProgram.h"
#include "Gamani.h"
#include "ADPCompiler.h"

Autopilot::Autopilot(Ship* ship):ship_(ship),ref_(NULL),lastError_("No errors"),adp_(new ADP())
{

}

Autopilot::~Autopilot()
{
  stopProg();
}

void Autopilot::stopProg()
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

void Autopilot::clearQueue()
{
  while (queue_.size() > 0) {
    APProgram* prog = queue_.back();
    delete prog;
    queue_.pop_back();
  }
  assert (queue_.size() == 0);
}

void Autopilot::step()
{
  if (adp_->isRunning()) {
    for (int i=0; i<50; ++i) {
      adp_->step();
    }
  }

  if (programs_.size() == 0) {
    if (queue_.size() == 0) {
      return;
    }
    APProgram* nextProg = queue_.front();
    queue_.pop_front();
    programs_.push_back(nextProg);
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
  if (programs_.size() > 0) { //Active program may have been cancelled due to error
    programs_.front()->step();
  }
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
  prog->setImmediate();
}

void Autopilot::endCurrProg()
{
  APProgram* prog = programs_.front();
  programs_.pop_front();
  garbage_.push_front(prog);
  if (programs_.size() == 0 && queue_.size() == 0) {
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

CString Autopilot::getProgName(ProgID id)
{
  //  enum ProgID {NoProgram, KillRot, Rotate, Launch, Approach, Accelerate, Stop, EqSpeed, ProGrade, RetroGrade, Orbit, RotateTo, RotateFrom, PerprndOrbit, UnKnown};

  switch (id) {
  case NoProgram:
    return "No Program";
  case KillRot:
    return "Kill rotation";
  case Rotate:
    return "Rotation";
  case Launch:
    return "Launch";
  case Approach:
    return "Approach";
  case Accelerate:
    return "Accelerate";
  case Stop:
    return "Stop";
  case EqSpeed:
    return "Match speed";
  case ProGrade:
    return "Pro Grade";
  case RetroGrade:
    return "Retro Grade";
  case Orbit:
    return "Stab orbit";
  case RotateTo:
    return "Rotate to";
  case RotateFrom:
    return "Rotate from";
  case PerpendOrbit:
    return "Perp orbit";
  default:
    return "UnKnown";
  }
}

void Autopilot::enqueue(APProgram* prog)
{
  queue_.push_back(prog);
}

vector<CString> Autopilot::getPrograms()
{
  vector<CString> res;
  auto itr = queue_.begin();
  for (; itr != queue_.end(); ++itr) {
    APProgram* progItr = *itr;
    res.push_back(getProgName(progItr->getID()));
  }
  return res;
}

void Autopilot::loadProg(CString fileName)
{
  ADPCompiler compiler;
  char memory[32000];
  compiler.compile(fileName, memory, 32000);
  int lastAddr = compiler.getLastAddr();
  bool res = adp_->loadProgram(memory, lastAddr/8);
  assert(res);
}
