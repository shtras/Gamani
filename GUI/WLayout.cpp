#include "StdAfx.h"
#include "WLayout.h"
#include "Renderer.h"

WLayout::WLayout(int left, int top, int width, int height):
  top_(top), left_(left), height_(height), width_(width), visible_(false)
{
}

WLayout::~WLayout()
{
  list<Widget*>::iterator itr = widgets_.begin();
  for (; itr != widgets_.end(); ++itr) {
    Widget* widget = *itr;
    delete widget;
  }
}

void WLayout::render()
{
  assert(width_ > 0 && height_ > 0);
  Renderer::getInstance().requestViewPort(left_, top_, width_, height_);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor3f(0.3, 0.3, 0);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.99, -0.99, 0);
  glVertex3f(0.99, -0.99, 0);
  glVertex3f(0.99, 0.99, 0);
  glVertex3f(-0.99, 0.99, 0);
  glEnd();

  glColor4f(0.3, 0.3, 0, 0.4);
  glBegin(GL_POLYGON);
  glVertex3f(-1, -1, 0.1);
  glVertex3f(1, -1, 0.1);
  glVertex3f(1, 1, 0.1);
  glVertex3f(-1, 1, 0.1);
  glEnd();

  list<Widget*>::iterator itr = widgets_.begin();
  for (; itr != widgets_.end(); ++itr) {
    Widget* widget = *itr;
    if (!widget->isVisible()) {
      continue;
    }
    glPushMatrix();

    int left = widget->getLeft();
    int top = widget->getTop();
    int width = widget->getWidth();
    int height = widget->getHeight();

    double dLeft = left/(double)width_;
    double dTop = top/(double)height_;
    double dWidth = width/(double)width_;
    double dHeight = height/(double)height_;

    dTop = (dTop - dHeight) * 2.0 - 1.0;
    dLeft = dLeft * 2.0 - 1.0;
    dWidth = dWidth * 2.0;
    dHeight = dHeight * 2.0;
    glTranslatef(dLeft, dTop, 0);

    widget->render(dLeft, dTop, dWidth, dHeight);
    glPopMatrix();
  }

  list<Widget*>::iterator gcItr = widgetsToGC_.begin();
  for (; gcItr != widgetsToGC_.end(); ++gcItr) {
    Widget* widgetToGC = *gcItr;
    widgetToGC->collectGarbage();
  }
  widgetsToGC_.clear();
}

void WLayout::renderEnd()
{
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glEnable(GL_LIGHTING);
  Renderer::getInstance().resetViewPort();
}

void WLayout::addWidget(Widget* widget)
{
  widgets_.push_back(widget);
}

void WLayout::removeWidget(Widget* widget)
{
  widgets_.remove(widget);
}

void WLayout::setDimensions(int left, int top, int width, int height)
{
  top_ = top;
  left_ = left;
  width_ = width;
  height_ = height;
}

bool WLayout::isInside(int x, int y)
{
  return ((x >= left_) && (x <= left_+width_) && (y >= top_-height_) && (y <= top_));
}

bool WLayout::handleMouseClick(int x, int y)
{
  list<Widget*>::iterator itr = widgets_.begin();
  for (; itr != widgets_.end(); ++itr) {
    Widget* widget = *itr;
    if (widget->isInteractive() && widget->isInside(x, y) && widget->isVisible()) {
      widget->click();
      return true;
    }
  }
  return false;
}

