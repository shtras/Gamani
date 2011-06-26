#include "StdAfx.h"
#include "Renderer.h"
#include "Planet.h"

Planet::Planet()
{
  color_ = Vector3(0, 0, 1);
  radius_ = 10;
  mass_ = 1e3;
  type_ = PlanetType;
  atmRadius_ = 0;
  atmColor_ = Vector3(0,0,0);
}

Planet::~Planet()
{
  gluDeleteQuadric(quadric_);
  glDeleteTextures(1, &texture_);

}

void Planet::setName(CString name)
{
  //glEnable ( GL_TEXTURE_2D );

  name_ = name;
  //string texName = name_ + ".bmp";
  //texture_ = LoadBitmap11(texName.c_str());
  //int a = 0;
  //glDisable ( GL_TEXTURE_2D );

}

void Planet::render()
{
  DynamicBody::render();

  //if (!strcmp(name_.c_str(), "Earth")) {

  Camera& camera = Renderer::getInstance().getCamera();
  const double* dCamPos = camera.getPosition();
  Vector3 camPos = Vector3(-dCamPos[0], -dCamPos[1], /*dCamPos[2]*/0);
  double camDirAlpha = camera.getHeading();
  double camDirPhi = camera.getPitch();
  double camZoom = camera.getZoom();

  double ralpha = camDirAlpha * 3.14159265 / 180.0;
  double rphi = camDirPhi * 3.14159265 / 180.0;

  Vector3 camDir(sin(ralpha), cos(ralpha), sin(rphi));
  camDir *= 10/camZoom;

  camPos -= camDir;

  //dist *= 1/Renderer::getInstance().getCamera().getZoom();
  camPos *= 1/GLOBAL_MULT;
  double camDist = (camPos - coord_).getLength() - getRadius();


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
  glColor3f(1,1,1);
    

  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);


  glRotatef(190, 1, 0, 0);
   
  if (1 || camDist > 5) {
    //glColor4f(1,1,1,0.2);
    glRotatef(rotation_, 0, 0, 1);
    if (texture_ == (GLuint)-1) {
      quadric_ = gluNewQuadric();
      gluQuadricTexture( quadric_, GL_TRUE);
      CString texName = CString("Textures/") + name_ + CString(".bmp");
      texture_ = LoadBitmap11(texName);
    }
    glEnable ( GL_TEXTURE_2D );
    glBindTexture ( GL_TEXTURE_2D, texture_);
    gluSphere( quadric_, /*2000*/radius_*GLOBAL_MULT, 36, 72);
    glDisable ( GL_TEXTURE_2D );
  } else {
    Vector3 dir = coord_ - camPos;
    dir.normalize();
    glRotatef(asin(dir[1])*180.0/3.14159265, 0, 1, 0);
   // glRotatef(acos(dir[0])*180.0/3.14159265, 0, 0, 1);
    glColor3f(0,1,0);
    glBegin(GL_POLYGON);
    glVertex3f(0,-GLOBAL_MULT,-GLOBAL_MULT);
    glVertex3f(0,GLOBAL_MULT,-GLOBAL_MULT);
    glVertex3f(0,-GLOBAL_MULT,GLOBAL_MULT);
    glVertex3f(0,GLOBAL_MULT,GLOBAL_MULT);
    glEnd();
  }

  drawName();


  glPopMatrix();
  return;
}
