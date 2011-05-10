#include "StdAfx.h"
#include "Star.h"
#include "Renderer.h"

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

  Vector3 coord = getRealCoordinates(coord_);
  double dist = coord.getLength();

  double zoom = Renderer::getInstance().getCamera().getZoom();
  double distMOne = 1/dist * 1e8 * 0.5;
  if (zoom > distMOne) {
    glScalef(distMOne, distMOne, distMOne);
  } else {
    Renderer::getInstance().getCamera().applyZoom();
  }

  glColor3f(color_[0], color_[1], color_[2]);
  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);

  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);

  //cout << coord[0]*GLOBAL_MULT << " -- " << coord[1]*GLOBAL_MULT << " -- " << coord[2]*GLOBAL_MULT << endl;
  //if (zoom > 5000) {
  //  double newZoom = 5000/zoom;
  //  cout << newZoom << endl;
  //  glPushMatrix();
  //  glScalef(5000/zoom, 5000/zoom, 5000/zoom);
  //  glPopMatrix();
  //}


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

  GLUquadric* earth = gluNewQuadric();

  gluQuadricTexture( earth, GL_TRUE);
  if (texture_ == (GLuint)-1) {
    CString texName = CString("Textures/") + name_ + CString(".bmp");
    texture_ = LoadBitmap11(texName);
  }
  glEnable ( GL_TEXTURE_2D );
  glBindTexture ( GL_TEXTURE_2D, texture_);
  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  //Vector3 coord = getRealCoordinates(coord_);
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);
  //glRotatef(90, 0, 1, 0);
  glRotatef(190, 1, 0, 0);

  glRotatef(rotation_, 0, 0, 1);
  gluSphere( earth, radius_*GLOBAL_MULT, 36, 72);
  glDisable ( GL_TEXTURE_2D );


  glDisable(GL_LIGHT0);
  gluDeleteQuadric(earth);
  glPopMatrix();
}
