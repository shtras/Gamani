#include "StdAfx.h"
#include "Renderable.h"
#include "Renderer.h"


Vector3 getRealCoordinates(const Vector3& coord)
{
  const double* cameraPos = Renderer::getInstance().getCamera().getPosition();
  const double zoom = Renderer::getInstance().getCamera().getZoom();
  Vector3 res = coord;
  res[0] = cameraPos[0]/GLOBAL_MULT + res[0];
  res[1] = cameraPos[1]/GLOBAL_MULT + res[1];
  res[2] = cameraPos[2];
  res[2] /= zoom;
  return res;
}

Renderable::Renderable()
{
  rank_ = 10;
}


Renderable::~Renderable()
{
}
