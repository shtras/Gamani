#pragma once
#include "Camera.h"
#include "AstralBody.h"
#include "SkyBox.h"
#include "Planet.h"
#include "Renderable.h"
#include "ParticleManager.h"

using namespace std;

int LoadBitmap11(const char *filename);
int LoadBitmap22(const char *filename);

class Renderer
{
public:
  static Renderer& getInstance();
  bool init(HINSTANCE& hInstance);
  void render();
  void renderEnd();
  HWND& getHwnd() {return hWnd_;}
  int getWidth() {return width_;}
  int getHeight() {return height_;}
  Camera& getCamera() {return *camera_;}
  void changeRenderList(vector<AstralBody*>* newList);
  void textOutNoMove(double x, double y, double z, const char* format, ...);

  void textOut(double x, double y, double z, char* format, ...);
  void requestViewPort(double left, double top, double width, double height, bool square = false, bool rightAlign = false);
  void resetViewPort();
  CString formatDistance (double dist, int len = 4);
  CString formatVelocity (double vel, int len = 4);
  CString formatTime (double time, int len = 4);
  void resize (int width, int height);
  void updateParticles();
  ParticleManager* getParticleManager() {return particleManager_;}
private:
  Renderer();
  ~Renderer();

  bool initWindow();
  bool initOpenGL();

  void testCase();
  void checkAndDrawAtmosphere();
  void drawAtmosphere(Planet* planet, double dist, double angle);

  bool rankEnoughToRender(Renderable* object);

  Camera* camera_;
  ParticleManager* particleManager_;
  bool init_;
  HWND hWnd_;
  HINSTANCE* hInstance_;
  int height_;
  int width_;
  bool customViewPort_;

  SkyBox* skyBox_;

  vector<AstralBody*>* renderList_;
};
