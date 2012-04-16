#include "StdAfx.h"
#include "Station.h"
#include "Renderer.h"
#include "Gamani.h"

Station::Station()
{
  mass_ = 1;
  type_ = StationType;
  yaw_ = 0;
  dockingLightsOn_ = false;
  //rotationSpeed_ = 360 / 9580.0;
}

Station::~Station()
{

}

void Station::render()
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
    float ambient_light_color[] = {0.15f, 0.15f, 0.15f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light_color);
    model->draw();
    float light_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_color);
    //renderPort();

  } else {
    glutSolidCone(radius_*GLOBAL_MULT/4.0f, radius_*GLOBAL_MULT, 10, 5);
  }


//  glPushMatrix();
//  glDisable(GL_LIGHTING);
//  glColor3f(0.5, 1, 0.4);
//  glRotatef(90, 1, 0, 0);
////   glTranslatef(getDockingPort()[0], getDockingPort()[1], getDockingPort()[2]);
////   glRotatef(getPortAngle(), 0, 0, 1);
////   glRotatef(-90, 1, 0, 0);
//
//  for (int i=1; i<10; ++i) {
//    glPushMatrix();
//    glTranslatef(i*2, 0, 0);
//    glRotatef(getPortAngle(), 0, 0, 1);
//    glRotatef(-90, 1, 0, 0);
//    glBegin(GL_LINES);
//    glVertex3f(-0.1, 0, -0.1);
//    glVertex3f(-0.1, 0, 0.1);
//    glVertex3f(0.1, 0, -0.1);
//    glVertex3f(0.1, 0, 0.1);
//    glEnd();
//    //glutWireSphere(0.1, 10, 10);
//    glPopMatrix();
//  }
//  glEnable(GL_LIGHTING);
//  glPopMatrix();

  drawName();
  glPopMatrix();
}

void Station::dock(Ship* ship)
{
  dockedShips_.push_back(ship);
  //display_->setVisible(true);
  dockingLightsOn_ = false;
}

void Station::undock(Ship* ship)
{
  auto itr = dockedShips_.begin();
  for (; itr != dockedShips_.end(); ++itr) {
    Ship* shipItr = *itr;
    if (shipItr == ship) {
      dockedShips_.erase(itr);
      //display_->setVisible(false);
      break;
    }
  }
}
