#include "StdAfx.h"
#include "MissionManager.h"

MissionManager& MissionManager::getInstance()
{
  static MissionManager instance;
  return instance;
}

MissionManager::MissionManager():
  display_(NULL)
{

}

MissionManager::~MissionManager()
{

}

void MissionManager::setDisplay(MissionDisplay* display)
{
  assert(!display_);
  display_ = display;
}
