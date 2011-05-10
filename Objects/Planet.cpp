#include "StdAfx.h"
#include "Renderer.h"
#include "Planet.h"

Planet::Planet()
{
  color_ = Vector3(0, 0, 1);
  radius_ = 10;
  mass_ = 1e3;
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
    

    if (texture_ == (GLuint)-1) {
      quadric_ = gluNewQuadric();
      gluQuadricTexture( quadric_, GL_TRUE);
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
    gluSphere( quadric_, radius_*GLOBAL_MULT, 36, 72);
    glDisable ( GL_TEXTURE_2D );
    glPopMatrix();
    //gluDeleteQuadric(earth);
    return;
  //}
  //glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();

  //glColor3f(color_[0], color_[1], color_[2]);
  ////glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  //Vector3 coord = getRealCoordinates(coord_);
  //glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, coord[2]*GLOBAL_MULT);
  //glutSolidSphere(radius_*GLOBAL_MULT, 30, 30);

  //glPopMatrix();
}
