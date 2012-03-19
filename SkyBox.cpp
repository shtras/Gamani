#include "StdAfx.h"
#include "SkyBox.h"
#include "PngWrapper.h"
#include "Gamani.h"
#ifdef MINGW
extern "C" long _ftol( double ); //defined by VC6 C libs
extern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); }
#endif

SkyBox::SkyBox()
{
  up_ = new Texture();
  down_ = new Texture();
  left_ = new Texture();
  right_ = new Texture();
  front_ = new Texture();
  back_ = new Texture();

  up1_ = new Texture();
  down1_ = new Texture();
  left1_ = new Texture();
  right1_ = new Texture();
  front1_ = new Texture();
  back1_ = new Texture();
}

SkyBox::~SkyBox()
{
  delete up_;
  delete down_;
  delete left_;
  delete right_;
  delete front_;
  delete back_;

  delete up1_;
  delete down1_;
  delete left1_;
  delete right1_;
  delete front1_;
  delete back1_;
}

void SkyBox::initTextures()
{
  bool res = true;
  res &= up_->load("SkyBox/sb_top3.png");
  res &= down_->load("SkyBox/sb_bottom4.png");
  res &= left_->load("SkyBox/sb_right1.png");
  res &= right_->load("SkyBox/sb_left2.png");
  res &= front_->load("SkyBox/sb_front5.png");
  res &= back_->load("SkyBox/sb_back6.png");

  res &= up1_->load("SkyBox/sb_top3-1.png");
  res &= down1_->load("SkyBox/sb_bottom4-1.png");
  res &= left1_->load("SkyBox/sb_right1-1.png");
  res &= right1_->load("SkyBox/sb_left2-1.png");
  res &= front1_->load("SkyBox/sb_front5-1.png");
  res &= back1_->load("SkyBox/sb_back6-1.png");
  assert(res);
}

void SkyBox::draw()
{
  bool skybox1 = Gamani::getInstance().usingSkyBox1();
  double size = 1e5;
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  //glDisable(GL_DEPTH_TEST);

  //front
  if (skybox1) {
    front_->use();
  } else {
    front1_->use();
  }
  glBegin(GL_POLYGON);
  glTexCoord2f(0,0);
  glVertex3f(-size, -size, size);
  glTexCoord2f(0,1);
  glVertex3f(-size, size, size);
  glTexCoord2f(1,1);
  glVertex3f(size, size, size);
  glTexCoord2f(1,0);
  glVertex3f(size, -size, size);
  glEnd();
  //Right
  if (skybox1) {
    right_->use();
  } else {
    right1_->use();
  }
  glBegin(GL_POLYGON);
  glTexCoord2f(0,0);
  glVertex3f(-size, -size, -size);
  glTexCoord2f(0,1);
  glVertex3f(-size, size, -size);
  glTexCoord2f(1,1);
  glVertex3f(-size, size, size);
  glTexCoord2f(1,0);
  glVertex3f(-size, -size, size);
  glEnd();
  //Left
  if (skybox1) {
    left_->use();
  } else {
    left1_->use();
  }
  glBegin(GL_POLYGON);
  glTexCoord2f(0,0);
  glVertex3f(size, -size, size);
  glTexCoord2f(0,1);
  glVertex3f(size, size, size);
  glTexCoord2f(1,1);
  glVertex3f(size, size, -size);
  glTexCoord2f(1,0); //1,0
  glVertex3f(size, -size, -size);
  glEnd();
  //Top
  if (skybox1) {
    up_->use();
  } else {
    up1_->use();
  }
  glBegin(GL_POLYGON);
  glTexCoord2f(0,1);
  glVertex3f(-size, size, -size);
  glTexCoord2f(1,1);
  glVertex3f(size, size, -size);
  glTexCoord2f(1,0);
  glVertex3f(size, size, size);
  glTexCoord2f(0,0);
  glVertex3f(-size, size, size);
  glEnd();
  //Bottom
  if (skybox1) {
    down_->use();
  } else {
    down1_->use();
  }
  glBegin(GL_POLYGON);
  glTexCoord2f(0,0);
  glVertex3f(-size, -size, -size);
  glTexCoord2f(0,1);
  glVertex3f(-size, -size, size);
  glTexCoord2f(1,1);
  glVertex3f(size, -size, size);
  glTexCoord2f(1,0);
  glVertex3f(size, -size, -size);
  glEnd();
  //Back
  if (skybox1) {
    back_->use();
  } else {
    back1_->use();
  }
  glBegin(GL_POLYGON);
  glTexCoord2f(1,0);
  glVertex3f(-size, -size, -size);
  glTexCoord2f(0,0);
  glVertex3f(size, -size, -size);
  glTexCoord2f(0,1);
  glVertex3f(size, size, -size);
  glTexCoord2f(1,1);
  glVertex3f(-size, size, -size);
  glEnd();

  //glEnable(GL_DEPTH_TEST);
  //glClearDepth(1);

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

Texture::Texture()
{
}

Texture::~Texture()
{
  glDeleteTextures(1, &texture_);
}

bool Texture::load(CString name)
{
  const char* texName = name;
  texName = _strlwr(_strdup(texName));

  if (strstr(texName, ".png")) {
    loadPng(texName);
  } else {
    return false;
  }
  return true;
}

bool Texture::loadPng(const char* name)
{
  PngWrapper* wrapper = new PngWrapper(name);
  bool res = wrapper->ReadPng();
  if (!res) {
    return false;
  }
  int width = wrapper->GetWidth();
  int height = wrapper->GetHeight();
  GLubyte* texBytes = new GLubyte[width*height*4];

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int c = wrapper->GetValue(j, height - i - 1);
      texBytes[i*width*4 + j*4] = (GLubyte) GET_R(c);
      texBytes[i*width*4 + j*4 + 1] = (GLubyte) GET_G(c);
      texBytes[i*width*4 + j*4 + 2] = (GLubyte) GET_B(c);
      texBytes[i*width*4 + j*4 + 3] = (GLubyte) 255;
    }
  }
  glEnable(GL_TEXTURE_2D);

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBytes);

  glDisable(GL_TEXTURE_2D);

  delete[] texBytes;
  delete wrapper;
  return true;
}

void Texture::use()
{
  glBindTexture(GL_TEXTURE_2D, texture_);
}
