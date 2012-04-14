#pragma once
#include "WLayout.h"

class MainMenu: public WLayout
{
public:
  MainMenu();
  ~MainMenu();
  void init();
  void render();
private:
  WButton* newGameButton_;
  WButton* loadButton_;
  WButton* exitButton_;

  void newGameButtonClick();
  void exitButtonClick();
  void loadButtonClick();
};
