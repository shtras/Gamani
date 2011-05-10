#include "StdAfx.h"
#include "GUIRenderer.h"


GUIRenderer::GUIRenderer()
{
}


GUIRenderer::~GUIRenderer()
{
}

void GUIRenderer::render(WLayout* layout)
{
  layout->render();
  layout->renderEnd();
}
