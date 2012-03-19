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
  double distMOne = 1/dist * 1e8 * 0.9;
  distMOne = 1 / pow(dist, 1/3.0) * 1e7 * 0.5;
  if (zoom > distMOne) {
    glScalef(distMOne, distMOne, distMOne);
  } else {
    Renderer::getInstance().getCamera().applyZoom();
  }
  glColor3f(1,1,1);
    

  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);

  //glEnable(GL_COLOR_SUM_EXT); 
  //glColor4f(0, 0.3, 0.50,1);
  //glSecondaryColor3fEXT(1, 1, 0.8);
  //glMaterialfv(GL_FRONT, GL_SPECULAR, mcolor);
  //glMateriali(GL_FRONT, GL_SHININESS, 96);
  //float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  //glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
  glRotatef(180, 1, 0, 0);
   
  if (1 || camDist > 5) {
    int slices = 32;
    int stacks = 64;
    if (camDist < 10) {
      slices = 200;
      stacks = 200;
    } else if (camDist > 1e3) {
      slices = 10;
      stacks = 10;
    }
    //glColor4f(1,1,1,0.2);
    //yaw_ = 360 - rotation_;
    glRotatef(yaw_, 0, 0, -1);
    if (texture_ == (GLuint)-1) {
      quadric_ = gluNewQuadric();
      gluQuadricTexture( quadric_, GL_TRUE);
      CString texName = CString("Textures/") + name_ + CString(".bmp");
      texture_ = LoadBitmap11(texName);
    }
    glEnable ( GL_TEXTURE_2D );
    glBindTexture ( GL_TEXTURE_2D, texture_);
    gluSphere( quadric_, /*2000*/radius_*GLOBAL_MULT, slices, stacks);
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
  //glDisable(GL_COLOR_SUM_EXT); 

  drawName();


  glPopMatrix();
  return;
}
