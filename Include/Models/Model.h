#pragma once


class ModelObject;
class Face;

struct Box
{
  Vector3 min;
  Vector3 max;
};

struct VBOVertex
{
  //0
  GLfloat vertex[3];
  //12
  GLfloat color[4];
  //28
  GLfloat normal[3];
  //40
  GLfloat texCoord[2];
  //48
  GLubyte pad[16];
  //64
};

struct Color
{
  float r;
  float g;
  float b;
  float a;
};

struct TexCoord
{
  float u;
  float v;
};

struct Material
{
  CString name;
  Color color;
  bool textured;
};

class Model
{
  friend class Wrapper3DS;
  friend class ModelObject;
public:
  Model();
  ~Model();
  Box getBoundingBox();
  void initVBOs();
  void draw() const;
  void normalize();
private:
  vector<ModelObject*> objects_;
  vector<Material*> materials_;
  CString name_;
  int numVertexes_;
  int numFaces_;
  Vector3 coord_;
  Matrix4 rotataion_;
  Vector3 min_;
  Vector3 max_;
};

class ModelObject
{
  friend class Wrapper3DS;
  friend class Model;
public:
  ModelObject(Model* m);
  ~ModelObject();
  Box getBoundingBox();
  void initVBO();
  void draw() const;
private:
  void normalize(float scale);
  void updateMinMax();
  //Offset from model coordinates
  Vector3 coord_;
  Matrix4 rotation_;
  Model* model_;

  vector<Vector3*> vertices_;
  vector<Vector3*> normals_;
  vector<TexCoord*> texCoords_;
  vector<int> matIndexes_;
  vector<Face*> faces_;

  VBOVertex* verts_;
  GLuint* indexes_;
  GLuint indexVBOID;
  GLuint modelVBO;
  int numVertexes_;
  int numFaces_;
  Vector3 min_;
  Vector3 max_;
};

class Face
{
  friend class Wrapper3DS;
  friend class ModelObject;
public:
  Face();
  ~Face();
private:
  vector<int> vertIndexes_;
  int matIndex_;
};
