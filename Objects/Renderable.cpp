#include "StdAfx.h"
#include "Renderable.h"
#include "Renderer.h"


Vector3 getRealCoordinates(const Vector3& coord)
{
  const double* cameraPos = Renderer::getInstance().getCamera().getPosition();
  const double zoom = Renderer::getInstance().getCamera().getZoom();
  Vector3 res = coord;
  double z = res[2];
  res[0] = cameraPos[0]/GLOBAL_MULT + res[0];
  res[1] = cameraPos[1]/GLOBAL_MULT + res[1];
  res[2] = cameraPos[2]/GLOBAL_MULT + res[2];
  res[2] += 10/zoom;
  //res[2] += z;
  return res;
}

Renderable::Renderable()
{
  rank_ = 10;
}


Renderable::~Renderable()
{
}
