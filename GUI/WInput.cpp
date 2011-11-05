#include "StdAfx.h"
#include "WInput.h"
#include "Renderer.h"

WInput::WInput():content_(""),focus_(false)
{

}

WInput::~WInput()
{

}

void WInput::render(double left, double top, double width, double height)
{
  glDisable(GL_LIGHTING);
  glColor3f(0.3, 0.4, 0.6);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, 0);
  glVertex3f(width, 0, 0);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();


  glColor4f(0.02,0.11,0.25, 0.9);
  glBegin(GL_POLYGON);
  glVertex3f(0, 0, 0);
  glColor4f(0.4,0.15,0.35, 0.9);
  glVertex3f(width, 0, 0);
  glVertex3f(width, height, 0);
  glVertex3f(0, height, 0);
  glEnd();
  glEnable(GL_LIGHTING);
  glColor3f(0.8,0.8,0.8);
  CString textToShow = content_;
  if (focus_) {
    textToShow = textToShow + CString("|");
  }
  Renderer::getInstance().textOut(left, top+height/2, -0.1, "%s", textToShow.operator const char *());
  glEnable(GL_LIGHTING);
}

bool WInput::handlePressedKey(int key)
{
  if (isNumberChar(key) || isLetterChar(key)) {
    char c = (char)key;
    content_ = content_ + CString(c);
    return true;
  }
  if (key == 0x8 && content_.getSize() > 0) { //backspace
    content_ = content_.left(content_.getSize()-1);
    return true;
  }
  return false;
}

void WInput::click()
{

}

void WInput::getFocus()
{
  focus_ = true;
}

void WInput::loseFocus()
{
  focus_ = false;
}
