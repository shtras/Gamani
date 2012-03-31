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
  void draw();
private:
  SphereVBO();
  ~SphereVBO();

  VBOVertex* verts_;
  GLuint* indexes_;
  GLuint indexVBOID;
  GLuint modelVBO;
  bool initialized_;
  int numFaces_;
};
