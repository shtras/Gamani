#include "StdAfx.h"
#include "Dockable.h"

void Dockable::renderPort()
{
  glPushMatrix();
  glRotatef(90, 1, 0, 0);
  glTranslatef(dockingPort_[0], dockingPort_[1], dockingPort_[2]);
  glRotatef(portAngle_, 0, 0, 1);
  glRotatef(-90, 1, 0, 0);
  glutSolidCone(0.1, 0.3, 10, 10);
  glPopMatrix();
}
