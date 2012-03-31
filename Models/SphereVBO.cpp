#include "StdAfx.h"
#include "SphereVBO.h"

SphereVBO::SphereVBO():initialized_(false)
{

}

SphereVBO::~SphereVBO()
{

}

void SphereVBO::initVBO()
{
  if (initialized_) {
    return;
  }
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  
  vector<Vector3> verts;
  vector<Vector3> normals;
  vector<Vector3> texCoords;
  vector<int> faces; //[i],[i+1],[i+2] -> face
  Vector3 firstVec = Vector3(0.0, 0.0, 1.0);
  Vector3 firstNormal = Vector3(0.0, 0.0, 1.0);
  Vector3 firstTexCoord = Vector3(0.5, 1.0, 0.0);
  Vector3 lastVec = Vector3(0.0, 0.0, -1.0);
  Vector3 lastNormal = Vector3(0.0, 0.0, -1.0);
  Vector3 lastTexCoord = Vector3(0.5, 0.0, 0.0);
  verts.push_back(firstVec);
  normals.push_back(firstNormal);
  texCoords.push_back(firstTexCoord);
  int slices = 50;
  int stacks = 50;
  int cnt = -1;
  double deltaJ = 180 / (double)stacks;
  for (double j=deltaJ; j<=180; j += deltaJ) {
    float radiusItr;// = sin(DegToRad(j));
    float height = cos(DegToRad(j));
    double stacksHalf = stacks/2.0;
    radiusItr = sqrt(1 - (height)*(height));
    for (int i=0; i<slices+1; ++i) {
      double z = height;
      double alpha = DegToRad(360.0 / (double)slices * (double)i);
      double x = sin(alpha) * radiusItr;
      double y = cos(alpha) * radiusItr;

      Vector3 norm = Vector3(x,y,z);
      Vector3 texCoord = Vector3(1.0 - i / (double)(slices), 1.0 - j/180.0, 0);
      texCoords.push_back(texCoord);
      norm.normalize();
      normals.push_back(norm);
      verts.push_back(Vector3(x,y,z));
      ++cnt;
      if (j == deltaJ && i != 0) {
        faces.push_back(0);
        faces.push_back(verts.size() - 1);
        faces.push_back(verts.size() - 2);
      } else if (j > deltaJ && i != 0 && j < 180-deltaJ) {
        assert (verts.size() > slices);
        faces.push_back(verts.size() - slices-3);
        faces.push_back(verts.size() - 1);
        faces.push_back(verts.size() - 2);

        faces.push_back(verts.size() - slices-3);
        faces.push_back(verts.size() - slices-2);
        faces.push_back(verts.size() - 1);
        if (j >= 180-2*deltaJ) {
          faces.push_back(verts.size() - 2);
          faces.push_back(verts.size() - 1);
          faces.push_back(-1);
        }
      } 
    }
  }
  verts.push_back(lastVec);
  normals.push_back(lastNormal);
  texCoords.push_back(lastTexCoord);

  
  verts_ = new VBOVertex[verts.size()];
  indexes_ = new GLuint[faces.size()];

  int idx = 0;
  int faceIdx = 0;

  for (unsigned int j=0; j<verts.size(); ++j) {
//     int idx = faces[j];
//     if (idx == -1) {
//       idx = verts.size();
//     }
    idx = j;
    Vector3 vertex = verts[idx];
    Vector3 normal = normals[idx];

    verts_[j].vertex[0] = vertex[0];
    verts_[j].vertex[1] = vertex[1];
    verts_[j].vertex[2] = vertex[2];

    Color col = {1,1,1,1};

    Vector3 tc = texCoords[idx];

    verts_[j].texCoord[0] = tc[0];
    verts_[j].texCoord[1] = tc[1];

    verts_[j].color[0] = col.r;
    verts_[j].color[1] = col.g;
    verts_[j].color[2] = col.b;
    verts_[j].color[3] = col.a;

    verts_[j].normal[0] = normal[0];
    verts_[j].normal[1] = normal[1];
    verts_[j].normal[2] = normal[2];
  }

  numFaces_ = faces.size();

  for (unsigned int i=0; i<faces.size(); ++i) {
    indexes_[i] = faces[i];
    if (faces[i] == -1) {
      indexes_[i] = verts.size() - 1;
    }
  }

  //glewInit();
  glGenBuffers(1, &modelVBO);
  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VBOVertex)*verts.size(), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VBOVertex)*verts.size(), verts_);


  glClientActiveTexture(GL_TEXTURE0);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(40));
  glClientActiveTexture(GL_TEXTURE1);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(40));


  glNormalPointer(GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(28));
  glColorPointer(4, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(12));
  glVertexPointer(3, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(0));

  glGenBuffers(1, &indexVBOID); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLuint), indexes_, GL_STATIC_DRAW);

  delete[] verts_;
  delete[] indexes_;

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  checkReleaseError("Error while initializing sphere VBO");
  initialized_ = true;
}

void SphereVBO::draw()
{
  if (!initialized_) {
    initVBO();
  }
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);

  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  glNormalPointer(GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(28));
  glColorPointer(4, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(12));
  glVertexPointer(3, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(0));

  glClientActiveTexture(GL_TEXTURE0);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(40));
  glClientActiveTexture(GL_TEXTURE1);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(40));

  glDrawElements(GL_TRIANGLES, numFaces_, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  checkError("Draw error");
}
