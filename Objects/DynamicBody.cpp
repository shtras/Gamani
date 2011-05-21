#include "StdAfx.h"
#include "DynamicBody.h"
#include "Renderer.h"
#include "Gamani.h"


DynamicBody::DynamicBody():AstralBody()
{
  velocity_[0] = 0;
  velocity_[1] = 0;
  velocity_[2] = 0;
  yawVel_ = 0;
  texture_ = (GLuint)-1;
  type_ = DynamicType;
}


DynamicBody::~DynamicBody()
{
}

void DynamicBody::render()
{
  AstralBody::render();
  if (!Gamani::getInstance().getTracers()) {
    return;
  }
  glPushMatrix();
  Renderer::getInstance().getCamera().applyZoom();
  glDisable(GL_LIGHTING);
  glColor3f(1,1,1);
  if (name_ == CString("Jupiter")) {
    glColor3f(1,1,1);
  } else if (name_ == CString("Io")) {
    glColor3f(1,0,0);
  } else if (name_ == CString("Europa")) {
    glColor3f(0,1,0);
  } else if (name_ == CString("Ganymede")) {
    glColor3f(0,0,1);
  } else if (name_ == CString("Callisto")) {
    glColor3f(1,0,1);
  }
  glBegin(GL_LINE_STRIP);
  list<Vector3>::iterator itr = points_.begin();
  for (; itr != points_.end(); ++itr) {
    Vector3 cp = *itr;
    Vector3 coord = getRealCoordinates(cp);
    glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);
    //glVertex3f(cp[0]*GLOBAL_MULT, cp[1]*GLOBAL_MULT, cp[2]*GLOBAL_MULT);
  }
  Vector3 coord = getRealCoordinates(coord_);
  glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);
  //glVertex3f(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  glEnd();
  glEnable(GL_LIGHTING);
  glPopMatrix();
}


void DynamicBody::snapshot()
{
 // if (name_ == "Galactica" || name_ == "Earth") {
    points_.push_back(Vector3(coord_));
 // }
  if (points_.size() > 1000) {
    points_.pop_front();
  }
}
