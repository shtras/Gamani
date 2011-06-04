#include "StdAfx.h"
#include "Dockable.h"

void Dockable::renderPort()
{
  glutSolidCone(0.1, 0.3, 10, 10);
}
