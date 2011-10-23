#include "StdAfx.h"
#include "MissionManager.h"

MissionManager& MissionManager::getInstance()
{
  static MissionManager instance;
  return instance;
}

MissionManager::MissionManager()
{

}

MissionManager::~MissionManager()
{

}
