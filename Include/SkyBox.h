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
};