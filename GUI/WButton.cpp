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
  Vector4 color1 = Vector4(0.12, 0.15, 0.35, 0.9);
  Vector4 color2 = Vector4(0.7, 0.21, 0.65, 0.9);
  if (isHovered()) {
    color1 = Vector4(0.17, 0.2, 0.4, 0.95);
    color2 = Vector4(0.75, 0.26, 0.7, 0.95);
  }
  if (isPressed()) {
    color1 = Vector4(0.22, 0.25, 0.45, 1.0);
    color2 = Vector4(0.8, 0.31, 0.75, 1.0);
  }
  glDisable(GL_LIGHTING);
  glColor3f(0.3, 0.4, 0.6);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, 0);
  glVertex3f(width-0.04, 0, 0);
  glVertex3f(width, 0.04, 0);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();


  glColor4f(color1[0], color1[1], color1[2], color1[3]);
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
