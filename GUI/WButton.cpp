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
  glColor3f(0.7, 0.4, 0.1);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, 0);
  glVertex3f(width, 0, 0);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();


  glColor3f(0.8,0.2,0);
  glBegin(GL_POLYGON);
  glVertex3f(0, 0, 0);
  glVertex3f(width, 0, 0);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();

  glColor3f(0.8,0.8,0.8);
  Renderer::getInstance().textOutNoMove(0.01, height/2, -0.1, "%s", label_.operator const char *());
}
