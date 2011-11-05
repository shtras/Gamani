#include "StdAfx.h"
#include "Model.h"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//////////////////////////////////////////////////////////////////////////
//Model
Model::Model()
{
  rotataion_.loadIdentity();
  min_ = Vector3(10000,10000,10000);
  max_ = Vector3(-10000,-10000,-10000);
}

Model::~Model()
{
  for (unsigned int i=0; i<objects_.size(); ++i) {
    delete objects_[i];
  }
  //delete &objects_;
  for (unsigned int i=0; i<materials_.size(); ++i) {
    delete materials_[i];
  }
}

void Model::initVBOs()
{
  for (unsigned int i=0; i<objects_.size(); ++i) {
    objects_[i]->initVBO();
  }
}

void Model::draw() const
{
  //glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, materials_[0]->texture[0]);
  glPushMatrix();
  glTranslatef(coord_[0], coord_[1], coord_[2]);
  for (unsigned int i=0; i<objects_.size(); ++i) {
    objects_[i]->draw();
  }
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  //glEnable(GL_LIGHTING);
}

void Model::normalize()
{
  min_ = Vector3(1000000,1000000,1000000);
  max_ = Vector3(-1000000,-1000000,-1000000);
  for (unsigned int i=0; i<objects_.size(); ++i) {
    objects_[i]->updateMinMax();
    ModelObject& object = *objects_[i];
    if (object.min_[0] < min_[0]) {
      min_[0] = object.min_[0];
    }
    if (object.min_[1] < min_[1]) {
      min_[1] = object.min_[1];
    }
    if (object.min_[2] < min_[2]) {
      min_[2] = object.min_[2];
    }
    if (object.max_[0] > max_[0]) {
      max_[0] = object.max_[0];
    }
    if (object.max_[1] > max_[1]) {
      max_[1] = object.max_[1];
    }
    if (object.max_[2] > max_[2]) {
      max_[2] = object.max_[2];
    }
  }
  float scalex = max_[0] - min_[0];
  float scaley = max_[1] - min_[1];
  float scalez = max_[2] - min_[2];
  float scale = (scalex < scaley) ? scaley : scalex;
  scale = (scale < scalez) ? scale : scalez;
  scale = 1.0f/scale;
  for (unsigned int i=0; i<objects_.size(); ++i) {
    objects_[i]->normalize(scale);
  }

  coord_[0] -= (max_[0] + min_[0])/2.0 * scale;
  coord_[1] -= (max_[1] + min_[1])/2.0 * scale;
  coord_[2] -= (max_[2] + min_[2])/2.0 * scale;
}

//////////////////////////////////////////////////////////////////////////
//ModelObejct
ModelObject::ModelObject(Model* m):
model_(m)
{
  rotation_.loadIdentity();
  min_ = Vector3(1000000,1000000,1000000);
  max_ = Vector3(-1000000,-1000000,-1000000);
}

ModelObject::~ModelObject()
{
  for (unsigned int i=0; i<vertices_.size(); ++i) {
    delete vertices_[i];
  }
  for (unsigned int i=0; i<normals_.size(); ++i) {
    delete normals_[i];
  }
  for (unsigned int i=0; i<texCoords_.size(); ++i) {
    delete texCoords_[i];
  }
  for (unsigned int i=0; i<faces_.size(); ++i) {
    delete faces_[i];
  }
  glDeleteBuffers(1, &indexVBOID);
  glDeleteBuffers(1, &modelVBO);
}

void ModelObject::draw() const
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(coord_[0], coord_[1], coord_[2]);

  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);

  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(40));
  glNormalPointer(GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(28));
  glColorPointer(4, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(12));
  glVertexPointer(3, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(0));

  glDrawElements(GL_TRIANGLES, numFaces_, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void ModelObject::updateMinMax()
{
  min_ = Vector3(1000000, 1000000, 1000000);
  max_ = Vector3(-1000000, -1000000, -1000000);
  for (unsigned int i=0; i<vertices_.size(); ++i) {
    Vector3& vertex = *vertices_[i];
    if (vertex[0] < min_[0]) {
      min_[0] = vertex[0];
    }
    if (vertex[1] < min_[1]) {
      min_[1] = vertex[1];
    }
    if (vertex[2] < min_[2]) {
      min_[2] = vertex[2];
    }

    if (vertex[0] > max_[0]) {
      max_[0] = vertex[0];
    }
    if (vertex[1] > max_[1]) {
      max_[1] = vertex[1];
    }
    if (vertex[2] > max_[2]) {
      max_[2] = vertex[2];
    }
  }
}

void ModelObject::normalize(float scale)
{
  for (unsigned int i=0; i<vertices_.size(); ++i) {
    *vertices_[i] *= scale;
  }
  coord_ *= scale;
}

void ModelObject::initVBO()
{
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glPolygonMode(GL_FRONT,GL_LINE);
  verts_ = new VBOVertex[numVertexes_];
  indexes_ = new GLuint[numFaces_];

  int idx = 0;
  int faceIdx = 0;
  int mymatidx = matIndexes_[0];
  assert(matIndexes_.size() == vertices_.size());

  for (unsigned int j=0; j<vertices_.size(); ++j) {
    if (mymatidx != matIndexes_[j]) {
      int a = matIndexes_[j];
      int b = 0;
    }
    Vector3* vertex = vertices_[j];
    Vector3* normal = normals_[j];

    verts_[idx].vertex[0] = (*vertex)[0];
    verts_[idx].vertex[1] = (*vertex)[1];
    verts_[idx].vertex[2] = (*vertex)[2];

    Color col = {1,1,1,1};
    if (matIndexes_[j] != -1) {
      Material* mat = model_->materials_[matIndexes_[j]];
      col.r = mat->color.r/255.0f;
      col.g = mat->color.g/255.0f;
      col.b = mat->color.b/255.0f;
      col.a = mat->color.a/255.0f;
    }

    verts_[idx].texCoord[0] = texCoords_[j]->u;
    verts_[idx].texCoord[1] = texCoords_[j]->v;

    verts_[idx].color[0] = col.r;
    verts_[idx].color[1] = col.g;
    verts_[idx].color[2] = col.b;
    verts_[idx].color[3] = col.a;

    verts_[idx].normal[0] = (*normal)[0];
    verts_[idx].normal[1] = (*normal)[1];
    verts_[idx].normal[2] = (*normal)[2];

    //indexes_[idx] = idx;
    ++idx;
  }
  for (unsigned int k=0; k<faces_.size(); ++k) {
    Face* face = faces_[k];
    assert(face->vertIndexes_.size() == 3);
    for (unsigned int l=0; l<face->vertIndexes_.size(); ++l) {
      indexes_[faceIdx] = face->vertIndexes_[l];
      //for (int m=0; m<face->vertIndexes_.getSize(); ++m) {
      //  if (face->matIndex_ == -1) {
      //    continue;
      //  }
      //  verts_[face->vertIndexes_[m]].color[0] = materials_[face->matIndex_]->color.r;
      //  verts_[face->vertIndexes_[m]].color[1] = materials_[face->matIndex_]->color.g;
      //  verts_[face->vertIndexes_[m]].color[2] = materials_[face->matIndex_]->color.b;
      //  verts_[face->vertIndexes_[m]].color[3] = materials_[face->matIndex_]->color.a;
      //}
      ++faceIdx;
    }
  }

  glewInit();
  glGenBuffers(1, &modelVBO);
  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VBOVertex)*numVertexes_, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VBOVertex)*numVertexes_, verts_);

  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(40));
  glNormalPointer(GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(28));
  glColorPointer(4, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(12));
  glVertexPointer(3, GL_FLOAT, sizeof(VBOVertex), BUFFER_OFFSET(0));

  glGenBuffers(1, &indexVBOID); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces_ * sizeof(GLuint), indexes_, GL_STATIC_DRAW);

  delete[] verts_;
  delete[] indexes_;
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}


//////////////////////////////////////////////////////////////////////////
//Face

Face::Face()
{

}

Face::~Face()
{
}
