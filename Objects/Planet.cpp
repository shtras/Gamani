#include "StdAfx.h"
#include "Renderer.h"
#include "Planet.h"
#include "Gamani.h"
#include "SphereVBO.h"

int LoadBitmap33(const char *filename)
{
  FILE * file;
  char temp;
  long i;

  unsigned char* infoheader_data;

  BITMAPINFOHEADER infoheader;

  GLuint num_texture;

  if( (file = fopen(filename, "rb"))==NULL) return (-1); // Open the file for reading

  fseek(file, 18, SEEK_CUR);  /* start reading width & height */
  fread(&infoheader.biWidth, sizeof(int), 1, file);

  fread(&infoheader.biHeight, sizeof(int), 1, file);

  fread(&infoheader.biPlanes, sizeof(short int), 1, file);
  if (infoheader.biPlanes != 1) {
    printf("Planes from %s is not 1: %u\n", filename, infoheader.biPlanes);
    return 0;
  }

  // read the bpp
  fread(&infoheader.biBitCount, sizeof(unsigned short int), 1, file);
  if (infoheader.biBitCount != 24) {
    printf("Bpp from %s is not 24: %d\n", filename, infoheader.biBitCount);
    return 0;
  }

  fseek(file, 24, SEEK_CUR);

  // read the data
  if(infoheader.biWidth<0){
    infoheader.biWidth = -infoheader.biWidth;
  }
  if(infoheader.biHeight<0){
    infoheader.biHeight = -infoheader.biHeight;
  }
  infoheader_data = (unsigned char *) malloc(infoheader.biWidth * infoheader.biHeight * 3);
  if (infoheader_data == NULL) {
    printf("Error allocating memory for color-corrected image data\n");
    return 0;
  }

  if ((i = fread(infoheader_data, infoheader.biWidth * infoheader.biHeight * 3, 1, file)) != 1) {
    printf("Error reading image data from %s.\n", filename);
    return 0;
  }

  for (i=0; i<(infoheader.biWidth * infoheader.biHeight * 3); i+=3) { // reverse all of the colors. (bgr -> rgb)
    temp = infoheader_data[i];
    infoheader_data[i] = infoheader_data[i+2];
    infoheader_data[i+2] = temp;
  }


  fclose(file); // Closes the file stream

  glGenTextures(1, &num_texture);
  glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

  // The next commands sets the texture parameters
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //The minifying function

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // Finally we define the 2d texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, infoheader.biWidth, infoheader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, infoheader_data);

  // And create 2d mipmaps for the minifying function
  //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader.biWidth, infoheader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, infoheader_data);

  free(infoheader_data); // Free the memory we used to load the texture

  return (num_texture); // Returns the current texture OpenGL ID
}

int LoadBitmap44(const char *filename)
{
  FILE * file;
  char temp;
  long i;

  unsigned char* infoheader_data;

  BITMAPINFOHEADER infoheader;

  GLuint num_texture;

  if( (file = fopen(filename, "rb"))==NULL) return (-1); // Open the file for reading

  fseek(file, 18, SEEK_CUR);  /* start reading width & height */
  fread(&infoheader.biWidth, sizeof(int), 1, file);

  fread(&infoheader.biHeight, sizeof(int), 1, file);

  fread(&infoheader.biPlanes, sizeof(short int), 1, file);
  if (infoheader.biPlanes != 1) {
    printf("Planes from %s is not 1: %u\n", filename, infoheader.biPlanes);
    return 0;
  }

  // read the bpp
  fread(&infoheader.biBitCount, sizeof(unsigned short int), 1, file);
  if (infoheader.biBitCount != 24) {
    printf("Bpp from %s is not 24: %d\n", filename, infoheader.biBitCount);
    return 0;
  }

  fseek(file, 24, SEEK_CUR);

  // read the data
  if(infoheader.biWidth<0){
    infoheader.biWidth = -infoheader.biWidth;
  }
  if(infoheader.biHeight<0){
    infoheader.biHeight = -infoheader.biHeight;
  }
  infoheader_data = (unsigned char *) malloc(infoheader.biWidth * infoheader.biHeight * 3);
  if (infoheader_data == NULL) {
    printf("Error allocating memory for color-corrected image data\n");
    return 0;
  }

  if ((i = fread(infoheader_data, infoheader.biWidth * infoheader.biHeight * 3, 1, file)) != 1) {
    printf("Error reading image data from %s.\n", filename);
    return 0;
  }

  for (i=0; i<(infoheader.biWidth * infoheader.biHeight * 3); i+=3) { // reverse all of the colors. (bgr -> rgb)
    temp = infoheader_data[i];
    infoheader_data[i] = infoheader_data[i+2];
    infoheader_data[i+2] = temp;
  }

  fclose(file); // Closes the file stream

  glGenTextures(1, &num_texture);

  glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

  // The next commands sets the texture parameters
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //The minifying function

  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,  GL_PRIMARY_COLOR_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_INTERPOLATE_EXT);

  // Finally we define the 2d texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, infoheader.biWidth, infoheader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, infoheader_data);

  // And create 2d mipmaps for the minifying function
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader.biWidth, infoheader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, infoheader_data);


  free(infoheader_data); // Free the memory we used to load the texture
  return (num_texture); // Returns the current texture OpenGL ID
}

Planet::Planet()
{
  color_ = Vector3(0, 0, 1);
  radius_ = 10;
  mass_ = 1e3;
  type_ = PlanetType;
  atmRadius_ = 0;
  atmColor_ = Vector3(0,0,0);
  texture1_ = (GLuint)-1;
}

Planet::~Planet()
{
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

  Camera& camera = Renderer::getInstance().getCamera();
  //const double* dCamPos = camera.getPosition();
  //Vector3 camPos = Vector3(-dCamPos[0], -dCamPos[1], /*dCamPos[2]*/0);
  //double camDirAlpha = camera.getHeading();
  //double camDirPhi = camera.getPitch();
  //double camZoom = camera.getZoom();

  //double ralpha = camDirAlpha * 3.14159265 / 180.0;
  //double rphi = camDirPhi * 3.14159265 / 180.0;

  //Vector3 camDir(sin(ralpha), cos(ralpha), sin(rphi));
  //camDir *= 10/camZoom;

  //camPos -= camDir;

  //camPos *= 1/GLOBAL_MULT;
  Vector3 camPos = camera.getActualCoords();
  double camDist = (camPos - coord_).getLength() - getRadius();


  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  Vector3 coord = getRealCoordinates(coord_);
  double dist = coord.getLength();

  double zoom = Renderer::getInstance().getCamera().getZoom();
  double distMOne = 1/dist * 1e8 * 0.9;
  distMOne = 1 / pow(dist, 1/3.0) * 1e7 * 0.5;
  if (zoom > distMOne) {
    glScalef(distMOne, distMOne, distMOne);
  } else {
    Renderer::getInstance().getCamera().applyZoom();
  }
  glColor3f(1,1,1);
    

  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, coord[2]*GLOBAL_MULT);

  glRotatef(180, 1, 0, 0);
   
  if (1 || camDist > 5) {
    int slices = 32;
    int stacks = 64;
    int mode = 2;
    if (camDist < 10) {
      slices = 200;
      stacks = 200;
      mode = 0;
    } else if (camDist > 1e3) {
      slices = 10;
      stacks = 10;
      mode = 1;
    }
    glRotatef(yaw_, 0, 0, -1);

    if (name_ == "Earth" && camDist < 30) {
      static GLuint nightTex = -1;

      if (texture_ == (GLuint)-1) {
        CString texName = CString("Textures/") + name_ + CString(".bmp");
        texture_ = LoadBitmap11(texName);
      }
      
      if (nightTex == -1) {
        glClientActiveTexture(GL_TEXTURE1);
        glActiveTexture(GL_TEXTURE1);
        nightTex = LoadBitmap44("Textures/earthnight.bmp");
      }
      glClientActiveTexture(GL_TEXTURE0);
      glActiveTexture(GL_TEXTURE0);
      glEnable ( GL_TEXTURE_2D );
      glBindTexture ( GL_TEXTURE_2D, texture_);

      glClientActiveTexture(GL_TEXTURE1);
      glActiveTexture(GL_TEXTURE1);
      glEnable ( GL_TEXTURE_2D );

      glBindTexture ( GL_TEXTURE_2D, nightTex);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glScalef(radius_*GLOBAL_MULT, radius_*GLOBAL_MULT, radius_*GLOBAL_MULT);
      SphereVBO::getInstance().draw(2, 0);
      glPopMatrix();
      glDisable ( GL_TEXTURE_2D );
      glClientActiveTexture(GL_TEXTURE0);
      glActiveTexture(GL_TEXTURE0);
      glDisable ( GL_TEXTURE_2D );

      if (texture1_ == (GLuint)-1) {
        CString texName = CString("Textures/Clouds.bmp");
        texture1_ = LoadBitmap22(texName);
      }
      glRotatef(cloudYaw_, 0, 0, 1);
      glEnable ( GL_TEXTURE_2D );
      glBindTexture ( GL_TEXTURE_2D, texture1_);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glScalef((radius_ + 0.01)*GLOBAL_MULT, (radius_ + 0.01)*GLOBAL_MULT*1.01, (radius_ + 0.01)*GLOBAL_MULT*1.01);
      SphereVBO::getInstance().draw(1, 0);
      glPopMatrix();
      glDisable ( GL_TEXTURE_2D );
      glEnable(GL_LIGHTING);
    } else {
      if (radius_ > 0.1 && texture_ == (GLuint)-1) {
        CString texName = CString("Textures/") + name_ + CString(".bmp");
        texture_ = LoadBitmap11(texName);
      }
      glEnable ( GL_TEXTURE_2D );
      glBindTexture ( GL_TEXTURE_2D, texture_);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glScalef(radius_*GLOBAL_MULT, radius_*GLOBAL_MULT, radius_*GLOBAL_MULT);
      SphereVBO::getInstance().draw(1, mode);
      glPopMatrix();

      glDisable ( GL_TEXTURE_2D );
    }
  } else {
    Vector3 dir = coord_ - camPos;
    dir.normalize();
    glRotatef(asin(dir[1])*180.0/3.14159265, 0, 1, 0);
    glColor3f(0,1,0);
    glBegin(GL_POLYGON);
    glVertex3f(0,-GLOBAL_MULT,-GLOBAL_MULT);
    glVertex3f(0,GLOBAL_MULT,-GLOBAL_MULT);
    glVertex3f(0,-GLOBAL_MULT,GLOBAL_MULT);
    glVertex3f(0,GLOBAL_MULT,GLOBAL_MULT);
    glEnd();
  }

  drawName();


  glPopMatrix();
  return;
}
