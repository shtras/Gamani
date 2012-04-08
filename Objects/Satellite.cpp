#include "StdAfx.h"
#include "Satellite.h"
#include "Renderer.h"
#include "Gamani.h"

Satellite::Satellite()
{
  type_ = SatelliteType;
  mass_ = 1e-10;
}

Satellite::~Satellite()
{

}

void Satellite::render()
{
  DynamicBody::render();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  Renderer::getInstance().getCamera().applyZoom();
  glColor3f(color_[0], color_[1], color_[2]);

  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  Vector3 coord = getRealCoordinates(coord_);
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, coord[2]*GLOBAL_MULT);

  glRotatef(-90, 1, 0, 0);
  glRotatef(180-yaw_, 0, 1, 0);
  //glTranslatef(0, 0, -radius_*GLOBAL_MULT/2.0);
  const Model* model = getModel();
  if (model) {
    glScalef(radius_*GLOBAL_MULT, radius_*GLOBAL_MULT, radius_*GLOBAL_MULT);
    if (Gamani::getInstance().drawBoundBoxes()) {
      glDisable(GL_LIGHTING);
      glutWireSphere(1, 10, 10);
      glEnable(GL_LIGHTING);
    }
    model->draw();
  } else {
    glutSolidCone(radius_*GLOBAL_MULT/4.0f, radius_*GLOBAL_MULT, 10, 5);
  }
  drawName();
  glPopMatrix();
}
