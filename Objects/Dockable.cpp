#include "StdAfx.h"
#include "Dockable.h"

void Dockable::renderPort()
{
  glPushMatrix();
  glRotatef(90, 1, 0, 0);
  glRotatef(portAngle_, 0, 0, 1);
  glRotatef(-90, 1, 0, 0);
  glutSolidCone(0.1, 0.3, 10, 10);
  glPopMatrix();
}
