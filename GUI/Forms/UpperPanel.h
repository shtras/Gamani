#pragma once
#include "WLayout.h"
#include "Widgets.h"

class UpperPanel: public WLayout
{
public:
  UpperPanel();
  ~UpperPanel();
  void render();
  void init();

  void speedUpButtonClick();
  void speedDownButtonClick();
  void pauseButtonClick();
  void minimizeButtonClick();
  void namesButtonClick();
  void tracksButtonClick();
  void hideGUIButtonClick();
  void exitButtonClick();
  void saveButtonClick();
private:
  bool minimized_;
  WButton* minimizeButton_;
  WButton* speedDownButton_;
  WButton* pauseButton_;
  WButton* speedUpButton_;
  WButton* namesButton_;
  WButton* tracksButton_;
  WButton* hideGUIButton_;
  WButton* exitButton_;
  WButton* saveButton_;
};
