#include "StdAfx.h"
#include "MissionDisplay.h"

MissionDisplay::MissionDisplay()
{

}

MissionDisplay::~MissionDisplay()
{

}

void MissionDisplay::init()
{
  visible_ = true;
}

void MissionDisplay::render()
{
  WLayout::render();
}
