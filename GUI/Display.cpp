#include "StdAfx.h"
#include "Display.h"


Display::Display():ship_(NULL)
{
}

Display::~Display()
{
}

void Display::assignTo(Ship* ship)
{
  ship_ = ship;
}
