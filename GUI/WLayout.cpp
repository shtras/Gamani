#include "StdAfx.h"
#include "WLayout.h"
#include "Renderer.h"

WLayout::WLayout(int left, int top, int width, int height):
  top_(top), left_(left), height_(height), width_(width), visible_(false), square_(false), rightAlign_(false),
  maxTop_(top),maxLeft_(left),maxHeght_(height),maxWidth_(width),minimized_(false),bottom_(false),hovered_(false)
{
  minimizeButton_ = new WButton();
  minimizeButton_->setDimensions(0.01,0.95,0.05,0.04);
  minimizeButton_->setLabel("*");
  minimizeButton_->sigClick.connect(this, &WLayout::minimize);
  minimizeButton_->setVisible(true);
  addWidget(minimizeButton_);
}

WLayout::~WLayout()
{
  list<Widget*>::iterator itr = widgets_.begin();
  for (; itr != widgets_.end(); ++itr) {
    Widget* widget = *itr;
    delete widget;
  }
}

void WLayout::minimize()
{
  if (!minimized_) {
    maxHeght_ = height_;
    maxWidth_ = width_;
    maxTop_ = top_;
    if (bottom_) {
      top_ = 0.02;
    }
    height_ = 0.02;
    width_ = 0.02;
    list<Widget*>::iterator itr = widgets_.begin();
    for (; itr != widgets_.end(); ++itr) {
      Widget* w = *itr;
      if (w == minimizeButton_) {
        w->setDimensions(0.01,0.01,0.95,0.95);
        continue;
      }
      //w->setVisible(false);
    }
    minimized_ = true;
  } else {
    height_ = maxHeght_;
    width_ = maxWidth_;
    top_ = maxTop_;
    list<Widget*>::iterator itr = widgets_.begin();
    for (; itr != widgets_.end(); ++itr) {
      Widget* w = *itr;
      if (w == minimizeButton_) {
        w->setDimensions(0.01,0.95,0.05,0.05);
        continue;
      }
      //w->setVisible(true);
    }
    minimized_ = false;
  }
}

void WLayout::render()
{
  Vector4 color = Vector4(0.2, 0.12, 0.45, 0.2);
  if (isHovered()) {
    color = Vector4(0.22, 0.15, 0.47, 0.22);
  }
  assert(width_ > 0 && height_ > 0);
  Renderer::getInstance().requestViewPort(left_, top_, width_, height_, square_, rightAlign_);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glBegin(GL_POLYGON);
  //glColor4f(0.4, 0.32, 0.45, 0.4);
  glColor4f(color[0], color[1], color[2], color[3]);
  glVertex3f(-1, -1, 0.1);
  glVertex3f(1, -1, 0.1);
  glVertex3f(1, 1, 0.1);
  glVertex3f(-1, 1, 0.1);
  glEnd();

  glColor3f(0.1, 0.05, 0.4);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.99, -0.99, 0);
  glVertex3f(0.99, -0.99, 0);
  glVertex3f(0.99, 0.99, 0);
  glVertex3f(-0.99, 0.99, 0);
  glEnd();

  list<Widget*>::iterator itr = widgets_.begin();
  for (; itr != widgets_.end(); ++itr) {
    Widget* widget = *itr;
    if (!widget->isVisible()) {
      continue;
    }
    if (minimized_ && widget != minimizeButton_) {
      continue;
    }
    glPushMatrix();

    double dLeft = widget->getLeft();
    double dTop = widget->getTop();
    double dWidth = widget->getWidth();
    double dHeight = widget->getHeight();

    //double dLeft = left/(double)width_;
    //double dTop = top/(double)height_;
    //double dWidth = width/(double)width_;
    //double dHeight = height/(double)height_;

    dTop = (dTop/* - dHeight*/) * 2.0 - 1.0;
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

void WLayout::setDimensions(double left, double top, double width, double height)
{
  top_ = top;
  left_ = left;
  width_ = width;
  height_ = height;
}

bool WLayout::isInside(double x, double y)
{
  if (rightAlign_) {
    if (square_) {
      double trueWidth = width_ * Renderer::getInstance().getHeight() / (double)Renderer::getInstance().getWidth();
      x -= (1 - trueWidth);
    } else {
      x -= (1 - width_);
    }
  } else {
    x -= left_;
  }
  y -= top_-height_;
  if (square_) {
    x *= Renderer::getInstance().getWidth() / (double)Renderer::getInstance().getHeight();
  }
  return ((x >= 0) && (x <= width_) && (y >= 0) && (y <= height_));
}

Widget* WLayout::handleMouseEvent(UINT message, WPARAM wparam, double x, double y)
{
  list<Widget*>::iterator itr = widgets_.begin();
  for (; itr != widgets_.end(); ++itr) {
    Widget* widget = *itr;
    if (widget->isInside(x, y)) {
      widget->setHovered(true);
      if (widget->isInteractive() && widget->isVisible()) {
        if (message == WM_LBUTTONDOWN) {
          widget->setPressed(true);
        } else if (message == WM_LBUTTONUP && widget->isPressed()) {
          widget->click();
          widget->setPressed(false);
        }
        return widget;
      }
    } else {
      widget->setPressed(false);
      widget->setHovered(false);
    }

  }
  return NULL;
}

void WLayout::setHovered(bool val)
{
  hovered_ = val;
  if (!val) {
    list<Widget*>::iterator itr = widgets_.begin();
    for (; itr != widgets_.end(); ++itr) {
      Widget* widget = *itr;
      widget->setHovered(false);
      widget->setPressed(false);
    }
  }
}

