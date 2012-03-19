#include "StdAfx.h"
#include "LayoutManager.h"
#include "Renderer.h"

LayoutManager::LayoutManager():renderer_(NULL), focusOn_(NULL)
{
}

LayoutManager::~LayoutManager()
{
  delete renderer_;
  set<WLayout*>::iterator itr = layouts_.begin();
  for (; itr != layouts_.end(); ++itr) {
    WLayout* layout = *itr;
    delete layout;
  }
}

void LayoutManager::addLayout(WLayout* layout)
{
  assert (layouts_.count(layout) == 0);
  layouts_.insert(layout);
}

void LayoutManager::removeLayout(WLayout* layout)
{
  assert(layouts_.count(layout) == 1);
  layouts_.erase(layout);
}

void LayoutManager::setRenderer(GUIRenderer* renderer)
{
  delete renderer_;
  renderer_ = renderer;
}

void LayoutManager::render()
{
  assert(renderer_);
  set<WLayout*>::iterator itr = layouts_.begin();
  for (; itr != layouts_.end(); ++itr) {
    WLayout* layout = *itr;
    if (layout->isVisible()) {
      renderer_->render(layout);
    }
  }
}

bool LayoutManager::handleMouseEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (1 || wParam == MK_LBUTTON) {
    if (focusOn_) {
      focusOn_->loseFocus();
      focusOn_ = NULL;
    }
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);
    double dx = xPos / (double)Renderer::getInstance().getWidth();
    double dy = yPos / (double)Renderer::getInstance().getHeight();
    Renderer::getInstance().getHeight();
    //cout << xPos << " -- " << yPos << endl;
    //yPos = Renderer::getInstance().getHeight() - yPos;
    dy = 1 - dy;
    set<WLayout*>::iterator itr = layouts_.begin();
    for (; itr != layouts_.end(); ++itr) {
      double localDX = dx;
      double localDY = dy;
      WLayout* layout = *itr;
      if (layout->isVisible() && layout->isInside(localDX, localDY)) {
        layout->setHovered(true);
        //cout << "Yes!" << endl;
        if (layout->isRightAlign()) {
          double trueWidth = layout->getWidth();
          if (layout->isSquare()) {
            trueWidth *= Renderer::getInstance().getHeight() / (double)Renderer::getInstance().getWidth();
          }
          localDX -= (1 - trueWidth);
        } else {
          localDX -= layout->getLeft();
        }
        localDY -= layout->getBottom();
        localDX /= layout->getWidth();
        localDY /= layout->getHeight();
        if (layout->isSquare()) {
          localDX *= Renderer::getInstance().getWidth() / (double)Renderer::getInstance().getHeight();
        }
        Widget* res = layout->handleMouseEvent(message, wParam, localDX, localDY);
        if (res) {
          if (res->grabsFocus()) {
            res->getFocus();
            focusOn_ = res;
          }
          return true;
        }
      } else {
        layout->setHovered(false);
      }
    }
  }
  return false;
}

bool LayoutManager::handlePressedKey(int key)
{
  assert(focusOn_);
  return focusOn_->handlePressedKey(key);
}

bool LayoutManager::handleMessage(UINT message,WPARAM wParam,LPARAM lParam)
{
  switch (message) {
  case WM_LBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_MOUSEMOVE:
    return handleMouseEvent(message, wParam, lParam);
  }
  return false;
}
