#include "StdAfx.h"
#include "WButton.h"
#include "Renderer.h"

WButton::WButton()
{
}

WButton::~WButton()
{
}

void WButton::click()
{
  sigClick.emit();
}

void WButton::render(double left, double top, double width, double height)
{
  glDisable(GL_LIGHTING);
  glColor3f(0.3, 0.4, 0.6);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, 0);
  glVertex3f(width-0.04, 0, 0);
  glVertex3f(width, 0.04, 0);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();


  glColor4f(0.12,0.15,0.35, 0.9);
  glBegin(GL_POLYGON);
  glVertex3f(0, 0, 0);
  glVertex3f(width-0.04, 0, 0);
  glVertex3f(width, 0.04, 0);
  glColor4f(0.7,0.21,0.65, 0.9);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();

  glColor3f(0.8,0.8,0.8);
  Renderer::getInstance().textOut(left + width/10, top+height/2, -0.1, "%s", label_.operator const char *());
  glEnable(GL_LIGHTING);
}
