#include "StdAfx.h"
#include "LayoutManager.h"
#include "Renderer.h"

LayoutManager::LayoutManager():renderer_(NULL)
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
    renderer_->render(layout);
  }
}

bool LayoutManager::handleMouseClick(WPARAM wParam, LPARAM lParam)
{
  if (1 || wParam == MK_LBUTTON) {
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
      WLayout* layout = *itr;
      if (layout->isVisible() && layout->isInside(dx, dy)) {
        cout << "Yes!" << endl;
        dx -= layout->getLeft();
        dy -= layout->getBottom();
        dx /= layout->getWidth();
        dy /= layout->getHeight();
        if (layout->isSquare()) {
          dx *= Renderer::getInstance().getWidth() / (double)Renderer::getInstance().getHeight();
        }
        return layout->handleMouseClick(dx, dy);
      }
    }
  }
  return false;
}

bool LayoutManager::handleMessage(UINT message,WPARAM wParam,LPARAM lParam)
{
  switch (message) {
  case WM_LBUTTONUP:
    return handleMouseClick(wParam, lParam);
  }
  return false;
}
