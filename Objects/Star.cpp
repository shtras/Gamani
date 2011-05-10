#include "StdAfx.h"
#include "Star.h"


Star::Star()
{
  color_ = Vector3(1, 1, 0);
  mass_ = 198.0e18;
}


Star::~Star()
{
}

void Star::render()
{
  DynamicBody::render();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glColor3f(color_[0], color_[1], color_[2]);
  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  Vector3 coord = getRealCoordinates(coord_);
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);

  GLfloat light_position[] = { 0, 0, 0, 1};
  GLfloat light_color[] = { 1, 1, 1, 1.0f };
  GLfloat ambient_light_color[] = { 0, 0, 0, 1.0f };
  GLfloat test[] = { 0.05, 0.05, 0.05, 1.0f };
  glLightfv(GL_LIGHT1, GL_AMBIENT, test);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_color);
  glEnable(GL_LIGHT1);

  glEnable(GL_LIGHT0);
  glutSolidSphere(radius_*GLOBAL_MULT, 30, 30);
  glDisable(GL_LIGHT0);

  glPopMatrix();
}
