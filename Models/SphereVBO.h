#pragma once
#include "Model.h"

class SphereVBO
{
public:
  static SphereVBO& getInstance()
  {
    static SphereVBO instance;
    return instance;
  }

  void initVBO();
  void draw(int textures, int mode);
private:
  SphereVBO();
  ~SphereVBO();

  void initVBOParams(int textures, int slices, int stacks, GLuint& indexVBOID, GLuint& modelVBO, int& numFaces);

  GLuint indexVBOID_[3];
  GLuint modelVBO_[3];
  int numFaces_[3];

  bool initialized_;
};
