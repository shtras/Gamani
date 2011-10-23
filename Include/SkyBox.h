#pragma once

class Texture
{
public:
  Texture();
  ~Texture();
  bool load(CString name);
  void use();
private:
  bool loadPng(const char* name);

  GLuint texture_;
};

class SkyBox
{
public:
  SkyBox();
  ~SkyBox();
  void initTextures();
  void draw();
private:
  Texture* up_;
  Texture* down_;
  Texture* left_;
  Texture* right_;
  Texture* front_;
  Texture* back_;

  Texture* up1_;
  Texture* down1_;
  Texture* left1_;
  Texture* right1_;
  Texture* front1_;
  Texture* back1_;
};