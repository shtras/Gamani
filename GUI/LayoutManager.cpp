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
    cout << xPos << " -- " << yPos << endl;
    yPos = Renderer::getInstance().getHeight() - yPos;
    set<WLayout*>::iterator itr = layouts_.begin();
    for (; itr != layouts_.end(); ++itr) {
      WLayout* layout = *itr;
      if (layout->isInside(xPos, yPos)) {
        return layout->handleMouseClick(xPos - layout->getLeft(), yPos - layout->getBottom());
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
