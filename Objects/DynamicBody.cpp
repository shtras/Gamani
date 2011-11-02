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

void DynamicBody::renderAbsoluteOrbits()
{
  glPushMatrix();
  Renderer::getInstance().getCamera().applyZoom();
  glDisable(GL_LIGHTING);
  Vector3 color(0.4,0.8,0.1);
  if (name_ == CString("Io")) {
    color = Vector3(0.8,0,0);
  } else if (name_ == CString("Europa")) {
    color = Vector3(0,0.8,0);
  } else if (name_ == CString("Ganymede")) {
    color = Vector3(0,0,0.8);
  } else if (name_ == CString("Callisto")) {
    color = Vector3(0.8,0,0.8);
  }

  glBegin(GL_LINE_STRIP);
  list<Vector3>::iterator itr = points_.begin();
  int SZ = points_.size();
  double i=0;
  for (; itr != points_.end(); ++itr) {
    double transp = i/(double)SZ;
    i += 0.5;
    glColor4f(color[0],color[1],color[2],transp);
    Vector3 cp = *itr;
    Vector3 coord;
    coord = getRealCoordinates(cp);
    glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0);
  }
  Vector3 coord = getRealCoordinates(coord_);
  glColor4f(color[0],color[1],color[2],i/(double)SZ);
  glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0);
  glEnd();
  glLineWidth(1.0f);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void DynamicBody::renderRelativeOrbits()
{
  glPushMatrix();
  Renderer::getInstance().getCamera().applyZoom();
  glDisable(GL_LIGHTING);
  Vector3 color(0.4,0.8,0.1);
  if (name_ == CString("Io")) {
    color = Vector3(0.8,0,0);
  } else if (name_ == CString("Europa")) {
    color = Vector3(0,0.8,0);
  } else if (name_ == CString("Ganymede")) {
    color = Vector3(0,0,0.8);
  } else if (name_ == CString("Callisto")) {
    color = Vector3(0.8,0,0.8);
  }
  DynamicBody* orbitsAround = (DynamicBody*)Gamani::getInstance().getWorld()->getFollowedObject();
  assert(orbitsAround);
  const list<Vector3> pointsAround = orbitsAround->getPoints();
  assert (pointsAround.size() == points_.size());

  glBegin(GL_LINE_STRIP);
  list<Vector3>::iterator itr = points_.begin();
  list<Vector3>::const_iterator itrRef;
  itrRef = pointsAround.begin();
  int SZ = points_.size();
  double i=0;
  for (; itr != points_.end(); ++itr, ++itrRef) {
    assert(itrRef != pointsAround.end());
    Vector3 refCoord = *itrRef;
    double transp = i/(double)SZ;
    i += 0.5;
    glColor4f(color[0],color[1],color[2],transp);
    Vector3 cp = *itr;
    Vector3 coord;
    coord = cp - refCoord;
    glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0);
  }
  assert (itrRef == pointsAround.end());
  Vector3 coord;
  coord = coord_ - Gamani::getInstance().getWorld()->getFollowedObject()->getCoord();
  glColor4f(color[0],color[1],color[2],i/(double)SZ);
  glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0);
  glEnd();
  glLineWidth(1.0f);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void DynamicBody::render()
{
  AstralBody::render();
  if (!Gamani::getInstance().getTracers()) {
    return;
  }
  if (Gamani::getInstance().drawRelativeOrbits() && Gamani::getInstance().getWorld()->getFollowedObject()) {
    renderRelativeOrbits();
  } else {
    renderAbsoluteOrbits();
  }
}


void DynamicBody::snapshot()
{
 // if (name_ == "Galactica" || name_ == "Earth") {
  Vector3 coord = coord_;
  //if (Gamani::getInstance().getOrbitsAround()) {
  //  coord = coord - Gamani::getInstance().getOrbitsAround()->getCoord();
  //}
  points_.push_back(coord);
 // }
  if (points_.size() > 2000) {
    points_.pop_front();
  }
}

void DynamicBody::cleatTrack()
{
  while (points_.size() > 0) {
    points_.pop_back();
  }
}
