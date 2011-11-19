#include "StdAfx.h"
#include "Wrapper3DS.h"
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define warn( x )  message( __FILE__LINE__ #x "\n" ) 

// The chunk's id numbers
#define MAIN3DS         0x4D4D
#define MAIN_VERS       0x0002
#define EDIT3DS         0x3D3D
#define MESH_VERS       0x3D3E
#define OBJECT          0x4000
#define TRIG_MESH       0x4100
#define VERT_LIST       0x4110
#define FACE_DESC       0x4120
#define FACE_MAT        0x4130
#define TEX_VERTS       0x4140
#define SMOOTH_GROUP    0x4150
#define LOCAL_COORDS    0x4160
#define MATERIAL        0xAFFF
#define MAT_NAME        0xA000
#define MAT_AMBIENT     0xA010
#define MAT_DIFFUSE     0xA020
#define MAT_SPECULAR    0xA030
#define SHINY_PERC      0xA040
#define SHINY_STR_PERC  0xA041
#define TRANS_PERC      0xA050
#define TRANS_FOFF_PERC 0xA052
#define REF_BLUR_PERC   0xA053
#define RENDER_TYPE     0xA100
#define SELF_ILLUM      0xA084
#define MAT_SELF_ILPCT  0xA08A
#define WIRE_THICKNESS  0xA087
#define MAT_TEXMAP      0xA200
#define MAT_MAPNAME     0xA300
#define ONE_UNIT        0x0100
#define KEYF3DS         0xB000
#define FRAMES          0xB008
#define MESH_INFO       0xB002
#define HIER_POS        0xB030
#define HIER_FATHER     0xB010
#define PIVOT_PT        0xB013
#define TRACK00         0xB020
#define TRACK01         0xB021
#define TRACK02         0xB022
#define COLOR_RGB       0x0010
#define COLOR_TRU       0x0011
#define COLOR_TRUG      0x0012
#define COLOR_RGBG      0x0013
#define PERC_INT        0x0030
#define PERC_FLOAT      0x0031

Wrapper3DS* Wrapper3DS::instance_ = NULL;

Wrapper3DS::Wrapper3DS()
{
}

Wrapper3DS::~Wrapper3DS()
{
}

Model* Wrapper3DS::Load(CString fileName)
{
  path = NULL;
  Materials = NULL;
  Objects = NULL;


  Model* model = new Model();
  numMaterials = 0;
  numObjects = 0;

  totalVerts = 0;
  totalFaces = 0;
  char* name = new char[fileName.getSize()+1];
  strcpy(name, fileName);

  ChunkHeader main;
  if (strstr(name, "\"")) {
    name = strtok(name, "\"");
  }
  if (strstr(name, "/") || strstr(name, "\\"))
  {
    char *temp;
    if (strstr(name, "/")) {
      temp = strrchr(name, '/');
    } else {
      temp = strrchr(name, '\\');
    }
    char *src = name + strlen(name) - 1;
    while (!((*(src-1)) == '\\' || (*(src-1)) == '/')) {
      src--;
    }
    path = new char[strlen(name)-strlen(src)+1];
    memcpy (path, name, strlen(name) - strlen(src));
    path[src-name] = 0;
  }
  bin3ds = fopen(name, "rb");
  if (!bin3ds) {
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Failed to open model file: ") + fileName);
    return NULL;
  }
  fseek(bin3ds, 0, SEEK_SET);
  fread(&main.id,sizeof(main.id),1,bin3ds);
  fread(&main.len,sizeof(main.len),1,bin3ds);
  MainChunkProcessor(main.len, ftell(bin3ds));
  fclose(bin3ds);
  CalculateNormals();

  // For future reference
  model->name_ = name;

  // Find the total number of faces and vertices
  totalFaces = 0;
  totalVerts = 0;

  for (int i = 0; i < numObjects; i ++)
  {
    totalFaces += Objects[i].numFaces/3;
    totalVerts += Objects[i].numVerts;
  }

  // If the object doesn't have any texcoords generate some
  for (int k = 0; k < numObjects; k++)
  {
    if (Objects[k].numTexCoords == 0)
    {
      // Set the number of texture coords
      Objects[k].numTexCoords = Objects[k].numVerts;

      // Allocate an array to hold the texture coordinates
      Objects[k].TexCoords = new GLfloat[Objects[k].numTexCoords * 2]; //deleted

      // Make some texture coords
      for (int m = 0; m < Objects[k].numTexCoords; m++)
      {
        Objects[k].TexCoords[2*m] = Objects[k].Vertexes[3*m];
        Objects[k].TexCoords[2*m+1] = Objects[k].Vertexes[3*m+1];
      }
    }
  }

  // Let's build simple colored textures for the materials w/o a texture
  for (int j = 0; j < numMaterials; j++)
  {
    if (Materials[j].textured == false)
    {
      unsigned char r = Materials[j].color.r;
      unsigned char g = Materials[j].color.g;
      unsigned char b = Materials[j].color.b;
      Materials[j].tex.BuildColorTexture(r, g, b);
      Materials[j].textured = true;
    }
  }

  //////////////////////////////////////////////////////////////////////////
  //Building Model class
  model->numVertexes_ = totalVerts;
  model->numFaces_ = totalFaces;
  for (int i=0; i<numMaterials; ++i) {
    Material* mat = new Material();
    mat->color.r = Materials[i].color.r;
    mat->color.g = Materials[i].color.g;
    mat->color.b = Materials[i].color.b;
    mat->color.a = Materials[i].color.a;
    *mat->texture = *Materials[i].tex.texture;
    model->materials_.push_back(mat);
  }
  for (int i=0; i<numObjects; ++i) {
    Object& obj = Objects[i];
    ModelObject* modelObj = new ModelObject(model);
    modelObj->coord_[0] = obj.pos.x;
    modelObj->coord_[1] = obj.pos.y;
    modelObj->coord_[2] = obj.pos.z;
    assert(obj.numVerts == obj.numTexCoords);
    for (int j=0; j<obj.numVerts; ++j) {
      Vector3 vert = Vector3(obj.Vertexes[j*3], obj.Vertexes[j*3+1], obj.Vertexes[j*3+2]);
      Vector3 norm = Vector3(obj.Normals[j*3], obj.Normals[j*3+1], obj.Normals[j*3+2]);
      TexCoord tc;
      tc.u = obj.TexCoords[j*2];
      tc.v = obj.TexCoords[j*2+1];
      modelObj->vertices_.push_back(vert);
      modelObj->normals_.push_back(norm);
      modelObj->texCoords_.push_back(tc);
    }
    for (int j=0; j<obj.numMatFaces; ++j) {
      MaterialObject* matObj = new MaterialObject(modelObj);
      MaterialFaces& matFace = obj.MatFaces[j];
      for (int k=0; k<matFace.numSubFaces; k+=3) {
        int v1 = matFace.subFaces[k];
        int v2 = matFace.subFaces[k+1];
        int v3 = matFace.subFaces[k+2];
        matObj->faces_.push_back(v1);
        matObj->faces_.push_back(v2);
        matObj->faces_.push_back(v3);
      }
      matObj->matIdx_ = matFace.MatIndex;
      modelObj->objects_.push_back(matObj);
    }
    model->objects_.push_back(modelObj);

    //MaterialObject* modelObj = new MaterialObject(model);
    //modelObj->coord_[0] = obj.pos.x;
    //modelObj->coord_[1] = obj.pos.y;
    //modelObj->coord_[2] = obj.pos.z;
    //modelObj->numVertexes_ = obj.numVerts;
    //modelObj->numFaces_ = obj.numFaces;
    //for (int j=0; j<obj.numVerts; ++j) {
    //  Vector3* vert = new Vector3(obj.Vertexes[j*3], obj.Vertexes[j*3+1], obj.Vertexes[j*3+2]);
    //  modelObj->vertices_.push_back(vert);
    //  Vector3* norm = new Vector3(obj.Normals[j*3], obj.Normals[j*3+1], obj.Normals[j*3+2]);
    //  modelObj->normals_.push_back(norm);
    //  //modelObj->matIndexes_.push_back(-1);
    //}
    //for (int j=0; j<obj.numTexCoords; ++j) {
    //  TexCoord* texCoord = new TexCoord();
    //  texCoord->u = obj.TexCoords[j*2];
    //  texCoord->v = obj.TexCoords[j*2+1];
    //  modelObj->texCoords_.push_back(texCoord);
    //}
    //model->objects_.push_back(modelObj);
    //Face* face = NULL;
    //for (int j=0; j<obj.numFaces; ++j) {
    //  if (j%3 == 0 && j > 0) {
    //    modelObj->faces_.push_back(face);
    //  }
    //  if (j%3 == 0 && j != obj.numFaces - 1) {
    //    face = new Face();
    //  }
    //  face->vertIndexes_.push_back(obj.Faces[j]);
    //}
    //modelObj->faces_.push_back(face);
    ////assert(obj.numMatFaces == 1); //Currently supporting only single material per object
    //if (obj.numMatFaces != 1) {
    //  int a = 0;
    //}
    //int subFacesNum = 0;
    //for (int j=0; j<obj.numMatFaces; ++j) {
    //  MaterialFaces& matFaces = obj.MatFaces[j];
    //  modelObj->matIdx_ = matFaces.MatIndex;
    //  subFacesNum += matFaces.numSubFaces;
    //  //for (int k=0; k<matFaces.numSubFaces; ++k) {
    //  //  //modelObj->faces_[matFaces.subFaces[k]]->matIndex_ = matFaces.MatIndex;
    //  //  modelObj->matIndexes_[matFaces.subFaces[k]] = matFaces.MatIndex;
    //  //}
    //}
    //int a = 0;
  }
  model->normalize();
  delete[] path;
  delete[] name;
  delete[] Materials;
  for (int i=0; i<numObjects; ++i) {
    Object& obj = Objects[i];
    delete[] obj.Vertexes;
    delete[] obj.Normals;
    delete[] obj.TexCoords;
    delete[] obj.Faces;
    for (int j=0; j<obj.numMatFaces; ++j) {
      MaterialFaces& mf = obj.MatFaces[j];
      delete[] mf.subFaces;
    }
    delete[] obj.MatFaces;
  }
  delete[] Objects;
  Logger::getInstance().log(INFO_LOG_NAME, CString("Successfully loaded: ") + fileName);
  return model;
}

void Wrapper3DS::CalculateNormals()
{
  // Let's build some normals
  for (int i = 0; i < numObjects; i++)
  {
    for (int g = 0; g < Objects[i].numVerts; g++)
    {
      // Reduce each vert's normal to unit
      float length;
      Vector unit;

      unit.x = Objects[i].Normals[g*3];
      unit.y = Objects[i].Normals[g*3+1];
      unit.z = Objects[i].Normals[g*3+2];

      length = (float)sqrt((unit.x*unit.x) + (unit.y*unit.y) + (unit.z*unit.z));

      if (length == 0.0f)
        length = 1.0f;

      unit.x /= length;
      unit.y /= length;
      unit.z /= length;

      Objects[i].Normals[g*3]   = unit.x;
      Objects[i].Normals[g*3+1] = unit.y;
      Objects[i].Normals[g*3+2] = unit.z;
    }
  }
}

void Wrapper3DS::MainChunkProcessor(long length, long findex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      // This is the mesh information like vertices, faces, and materials
    case EDIT3DS :
      EditChunkProcessor(h.len, ftell(bin3ds));
      break;
      // I left this in case anyone gets very ambitious
    case KEYF3DS :
      //KeyFrameChunkProcessor(h.len, ftell(bin3ds));
      break;
    default   :
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::EditChunkProcessor(long length, long findex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // First count the number of Objects and Materials
  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case OBJECT:
      numObjects++;
      break;
    case MATERIAL:
      numMaterials++;
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // Now load the materials
  if (numMaterials > 0)
  {
    Materials = new Material3DS[numMaterials]; //deleted

    // Material is set to untextured until we find otherwise
    for (int d = 0; d < numMaterials; d++)
      Materials[d].textured = false;

    fseek(bin3ds, findex, SEEK_SET);

    int i = 0;

    while (ftell(bin3ds) < (findex + length - 6))
    {
      fread(&h.id,sizeof(h.id),1,bin3ds);
      fread(&h.len,sizeof(h.len),1,bin3ds);

      switch (h.id)
      {
      case MATERIAL :
        MaterialChunkProcessor(h.len, ftell(bin3ds), i);
        i++;
        break;
      default   :
        break;
      }

      fseek(bin3ds, (h.len - 6), SEEK_CUR);
    }
  }

  // Load the Objects (individual meshes in the whole model)
  if (numObjects > 0)
  {
    Objects = new Object[numObjects]; //deleted

    // Set the textured variable to false until we find a texture
    for (int k = 0; k < numObjects; k++) {
        /*
        char name[80];    // The object name
        float *Vertexes;   // The array of vertices
        float *Normals;    // The array of the normals for the vertices
        float *TexCoords;   // The array of texture coordinates for the vertices
        unsigned short *Faces;  // The array of face indices
        int numFaces;    // The number of faces
        int numMatFaces;   // The number of differnet material faces
        int numVerts;    // The number of vertices
        int numTexCoords;   // The number of vertices
        bool textured;    // True: the object has textures
        MaterialFaces *MatFaces; // The faces are divided by materials
        Vector pos;     // The position to move the object to
        Vector rot;     // The angles to rotate the object
        */
      Objects[k].textured = false;
      Objects[k].Vertexes = NULL;
      Objects[k].Normals = NULL;
      Objects[k].TexCoords = NULL;
      Objects[k].Faces = NULL;
      Objects[k].MatFaces = NULL;
      Objects[k].numFaces = Objects[k].numVerts = Objects[k].numTexCoords = Objects[k].numMatFaces = 0;
    }

    // Zero the objects position and rotation
    for (int m = 0; m < numObjects; m++)
    {
      Objects[m].pos.x = 0.0f;
      Objects[m].pos.y = 0.0f;
      Objects[m].pos.z = 0.0f;

      Objects[m].rot.x = 0.0f;
      Objects[m].rot.y = 0.0f;
      Objects[m].rot.z = 0.0f;
    }

    // Zero out the number of texture coords
    for (int n = 0; n < numObjects; n++)
      Objects[n].numTexCoords = 0;

    fseek(bin3ds, findex, SEEK_SET);

    int j = 0;

    while (ftell(bin3ds) < (findex + length - 6))
    {
      fread(&h.id,sizeof(h.id),1,bin3ds);
      fread(&h.len,sizeof(h.len),1,bin3ds);

      switch (h.id)
      {
      case OBJECT:
        ObjectChunkProcessor(h.len, ftell(bin3ds), j);
        j++;
        break;
      default:
        break;
      }

      fseek(bin3ds, (h.len - 6), SEEK_CUR);
    }
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::MaterialChunkProcessor(long length, long findex, int matindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case MAT_NAME:
      // Loads the material's names
      MaterialNameChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    case MAT_AMBIENT:
      //ColorChunkProcessor(h.len, ftell(bin3ds));
      break;
    case MAT_DIFFUSE:
      DiffuseColorChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    case MAT_SPECULAR:
      //ColorChunkProcessor(h.len, ftell(bin3ds));
    case MAT_TEXMAP:
      // Finds the names of the textures of the material and loads them
      TextureMapChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::MaterialNameChunkProcessor(long length, long findex, int matindex)
{
  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the material's name
  for (int i = 0; i < 80; i++)
  {
    Materials[matindex].name[i] = fgetc(bin3ds);
    if (Materials[matindex].name[i] == 0)
    {
      Materials[matindex].name[i] = NULL;
      break;
    }
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::DiffuseColorChunkProcessor(long length, long findex, int matindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    // Determine the format of the color and load it
    switch (h.id)
    {
    case COLOR_RGB:
      // A rgb float color chunk
      FloatColorChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    case COLOR_TRU:
      // A rgb int color chunk
      IntColorChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    case COLOR_RGBG:
      // A rgb gamma corrected float color chunk
      FloatColorChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    case COLOR_TRUG:
      // A rgb gamma corrected int color chunk
      IntColorChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::FloatColorChunkProcessor(long length, long findex, int matindex)
{
  float r;
  float g;
  float b;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  fread(&r,sizeof(r),1,bin3ds);
  fread(&g,sizeof(g),1,bin3ds);
  fread(&b,sizeof(b),1,bin3ds);

  Materials[matindex].color.r = (unsigned char)(r*255.0f);
  Materials[matindex].color.g = (unsigned char)(r*255.0f);
  Materials[matindex].color.b = (unsigned char)(r*255.0f);
  Materials[matindex].color.a = 255;

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::IntColorChunkProcessor(long length, long findex, int matindex)
{
  unsigned char r;
  unsigned char g;
  unsigned char b;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  fread(&r,sizeof(r),1,bin3ds);
  fread(&g,sizeof(g),1,bin3ds);
  fread(&b,sizeof(b),1,bin3ds);

  Materials[matindex].color.r = r;
  Materials[matindex].color.g = g;
  Materials[matindex].color.b = b;
  Materials[matindex].color.a = 255;

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::TextureMapChunkProcessor(long length, long findex, int matindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case MAT_MAPNAME:
      // Read the name of texture in the Diffuse Color map
      MapNameChunkProcessor(h.len, ftell(bin3ds), matindex);
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::MapNameChunkProcessor(long length, long findex, int matindex)
{
  char name[80];

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the name of the texture
  for (int i = 0; i < 80; i++)
  {
    name[i] = fgetc(bin3ds);
    if (name[i] == 0)
    {
      name[i] = NULL;
      break;
    }
  }

  // Load the name and indicate that the material has a texture
  char fullname[80];
  sprintf(fullname, "%s", name);
  bool res = Materials[matindex].tex.Load(fullname);
  Materials[matindex].textured = res;

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::ObjectChunkProcessor(long length, long findex, int objindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Load the object's name
  for (int i = 0; i < 80; i++)
  {
    Objects[objindex].name[i] = fgetc(bin3ds);
    if (Objects[objindex].name[i] == 0)
    {
      Objects[objindex].name[i] = NULL;
      break;
    }
  }

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case TRIG_MESH:
      // Process the triangles of the object
      TriangularMeshChunkProcessor(h.len, ftell(bin3ds), objindex);
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::TriangularMeshChunkProcessor(long length, long findex, int objindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);
  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case VERT_LIST:
      // Load the vertices of the onject
      VertexListChunkProcessor(h.len, ftell(bin3ds), objindex);
      break;
    case LOCAL_COORDS:
      //LocalCoordinatesChunkProcessor(h.len, ftell(bin3ds));
      break;
    case TEX_VERTS:
      // Load the texture coordinates for the vertices
      TexCoordsChunkProcessor(h.len, ftell(bin3ds), objindex);
      Objects[objindex].textured = true;
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // After we have loaded the vertices we can load the faces
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case FACE_DESC:
      // Load the faces of the object
      FacesDescriptionChunkProcessor(h.len, ftell(bin3ds), objindex);
      break;
    default:
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::VertexListChunkProcessor(long length, long findex, int objindex)
{
  unsigned short numVerts;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the number of vertices of the object
  fread(&numVerts,sizeof(numVerts),1,bin3ds);

  // Allocate arrays for the vertices and normals
  Objects[objindex].Vertexes = new GLfloat[numVerts * 3]; //deleted
  Objects[objindex].Normals = new GLfloat[numVerts * 3]; //deleted

  // Assign the number of vertices for future use
  Objects[objindex].numVerts = numVerts;

  // Zero out the normals array
  for (int j = 0; j < numVerts * 3; j++)
    Objects[objindex].Normals[j] = 0.0f;

  // Read the vertices, switching the y and z coordinates and changing the sign of the z coordinate
  for (int i = 0; i < numVerts * 3; i+=3)
  {
    fread(&Objects[objindex].Vertexes[i],sizeof(GLfloat),1,bin3ds);
    fread(&Objects[objindex].Vertexes[i+2],sizeof(GLfloat),1,bin3ds);
    fread(&Objects[objindex].Vertexes[i+1],sizeof(GLfloat),1,bin3ds);

    // Change the sign of the z coordinate
    Objects[objindex].Vertexes[i+2] = -Objects[objindex].Vertexes[i+2];
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::TexCoordsChunkProcessor(long length, long findex, int objindex)
{
  // The number of texture coordinates
  unsigned short numCoords;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the number of coordinates
  fread(&numCoords,sizeof(numCoords),1,bin3ds);

  // Allocate an array to hold the texture coordinates
  Objects[objindex].TexCoords = new GLfloat[numCoords * 2]; //deleted

  // Set the number of texture coords
  Objects[objindex].numTexCoords = numCoords;

  // Read teh texture coordiantes into the array
  for (int i = 0; i < numCoords * 2; i+=2)
  {
    fread(&Objects[objindex].TexCoords[i],sizeof(GLfloat),1,bin3ds);
    fread(&Objects[objindex].TexCoords[i+1],sizeof(GLfloat),1,bin3ds);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::FacesDescriptionChunkProcessor(long length, long findex, int objindex)
{
  ChunkHeader h;
  unsigned short numFaces; // The number of faces in the object
  unsigned short vertA;  // The first vertex of the face
  unsigned short vertB;  // The second vertex of the face
  unsigned short vertC;  // The third vertex of the face
  unsigned short flags;  // The winding order flags
  long subs;     // Holds our place in the file
  int numMatFaces = 0;  // The number of different materials

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the number of faces
  fread(&numFaces,sizeof(numFaces),1,bin3ds);

  // Allocate an array to hold the faces
  Objects[objindex].Faces = new GLushort[numFaces * 3]; //deleted
  // Store the number of faces
  Objects[objindex].numFaces = numFaces * 3;

  // Read the faces into the array
  for (int i = 0; i < numFaces * 3; i+=3)
  {
    // Read the vertices of the face
    fread(&vertA,sizeof(vertA),1,bin3ds);
    fread(&vertB,sizeof(vertB),1,bin3ds);
    fread(&vertC,sizeof(vertC),1,bin3ds);
    fread(&flags,sizeof(flags),1,bin3ds);

    // Place them in the array
    Objects[objindex].Faces[i]   = vertA;
    Objects[objindex].Faces[i+1] = vertB;
    Objects[objindex].Faces[i+2] = vertC;

    // Calculate the face's normal
    Vector n;
    Vertex v1;
    Vertex v2;
    Vertex v3;

    v1.x = Objects[objindex].Vertexes[vertA*3];
    v1.y = Objects[objindex].Vertexes[vertA*3+1];
    v1.z = Objects[objindex].Vertexes[vertA*3+2];
    v2.x = Objects[objindex].Vertexes[vertB*3];
    v2.y = Objects[objindex].Vertexes[vertB*3+1];
    v2.z = Objects[objindex].Vertexes[vertB*3+2];
    v3.x = Objects[objindex].Vertexes[vertC*3];
    v3.y = Objects[objindex].Vertexes[vertC*3+1];
    v3.z = Objects[objindex].Vertexes[vertC*3+2];

    // calculate the normal
    float u[3], v[3];

    // V2 - V3;
    u[0] = v2.x - v3.x;
    u[1] = v2.y - v3.y;
    u[2] = v2.z - v3.z;

    // V2 - V1;
    v[0] = v2.x - v1.x;
    v[1] = v2.y - v1.y;
    v[2] = v2.z - v1.z;

    n.x = (u[1]*v[2] - u[2]*v[1]);
    n.y = (u[2]*v[0] - u[0]*v[2]);
    n.z = (u[0]*v[1] - u[1]*v[0]);

    // Add this normal to its verts' normals
    Objects[objindex].Normals[vertA*3]   += n.x;
    Objects[objindex].Normals[vertA*3+1] += n.y;
    Objects[objindex].Normals[vertA*3+2] += n.z;
    Objects[objindex].Normals[vertB*3]   += n.x;
    Objects[objindex].Normals[vertB*3+1] += n.y;
    Objects[objindex].Normals[vertB*3+2] += n.z;
    Objects[objindex].Normals[vertC*3]   += n.x;
    Objects[objindex].Normals[vertC*3+1] += n.y;
    Objects[objindex].Normals[vertC*3+2] += n.z;
  }

  // Store our current file position
  subs = ftell(bin3ds);

  // Check to see how many materials the faces are split into
  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
    case FACE_MAT :
      //FacesMaterialsListChunkProcessor(h.len, ftell(bin3ds), objindex);
      numMatFaces++;
      break;
    default   :
      break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  Objects[objindex].MatFaces = NULL;

  // Split the faces up according to their materials
  if (numMatFaces > 0)
  {
    // Allocate an array to hold the lists of faces divided by material
    Objects[objindex].MatFaces = new MaterialFaces[numMatFaces]; //deleted
    // Store the number of material faces
    Objects[objindex].numMatFaces = numMatFaces;

    fseek(bin3ds, subs, SEEK_SET);

    int j = 0;

    // Split the faces up
    while (ftell(bin3ds) < (findex + length - 6))
    {
      fread(&h.id,sizeof(h.id),1,bin3ds);
      fread(&h.len,sizeof(h.len),1,bin3ds);

      switch (h.id)
      {
      case FACE_MAT :
        // Process the faces and split them up
        FacesMaterialsListChunkProcessor(h.len, ftell(bin3ds), objindex, j);
        j++;
        break;
      default   :
        break;
      }

      fseek(bin3ds, (h.len - 6), SEEK_CUR);
    }
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Wrapper3DS::FacesMaterialsListChunkProcessor(long length, long findex, int objindex, int subfacesindex)
{
  char name[80];    // The material's name
  unsigned short numEntries; // The number of faces associated with this material
  unsigned short Face;  // Holds the faces as they are read
  int material;    // An index to the Materials array for this material

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the material's name
  for (int i = 0; i < 80; i++)
  {
    name[i] = fgetc(bin3ds);
    if (name[i] == 0)
    {
      name[i] = NULL;
      break;
    }
  }

  // Faind the material's index in the Materials array
  for (material = 0; material < numMaterials; material++)
  {
    if (strcmp(name, Materials[material].name) == 0)
      break;
  }

  // Store this value for later so that we can find the material
  Objects[objindex].MatFaces[subfacesindex].MatIndex = material;

  // Read the number of faces associated with this material
  fread(&numEntries,sizeof(numEntries),1,bin3ds);

  // Allocate an array to hold the list of faces associated with this material
  Objects[objindex].MatFaces[subfacesindex].subFaces = new GLushort[numEntries * 3]; //deleted
  // Store this number for later use
  Objects[objindex].MatFaces[subfacesindex].numSubFaces = numEntries * 3;

  // Read the faces into the array
  for (int i = 0; i < numEntries * 3; i+=3)
  {
    // read the face
    fread(&Face,sizeof(Face),1,bin3ds);
    // Add the face's vertices to the list
    Objects[objindex].MatFaces[subfacesindex].subFaces[i] = Objects[objindex].Faces[Face * 3];
    Objects[objindex].MatFaces[subfacesindex].subFaces[i+1] = Objects[objindex].Faces[Face * 3 + 1];
    Objects[objindex].MatFaces[subfacesindex].subFaces[i+2] = Objects[objindex].Faces[Face * 3 + 2];
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}
