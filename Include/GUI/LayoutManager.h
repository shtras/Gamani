#pragma once
#include "WLayout.h"
#include "GUIRenderer.h"
#include <Windows.h>


class LayoutManager: public has_slots<>
{
public:
  LayoutManager();
  ~LayoutManager();

  void addLayout(WLayout* layout);
  void removeLayout(WLayout* layout);

  void render();
  void setRenderer(GUIRenderer* renderer);

  bool handleMessage(UINT message,WPARAM wParam,LPARAM lParam);
  bool focusGrabbed() {return focusOn_ != NULL;}
  bool handlePressedKey(int key);
private:
  bool handleMouseClick(WPARAM wParam, LPARAM lParam);

  set<WLayout*> layouts_;
  GUIRenderer* renderer_;
  Widget* focusOn_;
};

