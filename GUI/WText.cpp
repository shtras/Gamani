#include "StdAfx.h"
#include "WText.h"
#include "Renderer.h"

WText::WText()
{
}

WText::~WText()
{
}

void WText::render(double left, double top, double width, double height)
{
  Renderer::getInstance().textOut(left, top, 0, "%s", text_.operator const char *());
}
