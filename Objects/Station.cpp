#include "StdAfx.h"
#include "Station.h"

Station::Station()
{
  mass_ = 1;
}

Station::~Station()
{

}

void Station::render()
{
  DynamicBody::render();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glColor3f(color_[0], color_[1], color_[2]);

  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  Vector3 coord = getRealCoordinates(coord_);
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);

  glRotatef(-90, 1, 0, 0);
  glRotatef(180-yaw_, 0, 1, 0);
  //glTranslatef(0, 0, -radius_*GLOBAL_MULT/2.0);

  const Model* model = getModel();
  if (model) {
    glScalef(radius_*GLOBAL_MULT, radius_*GLOBAL_MULT, radius_*GLOBAL_MULT);
    model->draw();
  } else {
    glutSolidCone(radius_*GLOBAL_MULT/4.0f, radius_*GLOBAL_MULT, 10, 5);
  }

  glPopMatrix();
}
