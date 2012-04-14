#include "StdAfx.h"
#include "MainMenu.h"
#include "Gamani.h"

MainMenu::MainMenu()
{

}

MainMenu::~MainMenu()
{

}
void MainMenu::init()
{
  visible_ = true;
  newGameButton_ = new WButton();
  newGameButton_->setDimensions(0.1, 0.65, 0.8, 0.2);
  newGameButton_->setLabel("New game");
  newGameButton_->sigClick.connect(this, &MainMenu::newGameButtonClick);
  addWidget(newGameButton_);

  loadButton_ = new WButton();
  loadButton_->setDimensions(0.1, 0.4, 0.8, 0.2);
  loadButton_->setLabel("Load game");
  loadButton_->sigClick.connect(this, &MainMenu::loadButtonClick);
  addWidget(loadButton_);

  exitButton_ = new WButton();
  exitButton_->setDimensions(0.1, 0.15, 0.8, 0.2);
  exitButton_->setLabel("Exit");
  exitButton_->sigClick.connect(this, &MainMenu::exitButtonClick);
  addWidget(exitButton_);
}

void MainMenu::render()
{
  WLayout::render();
}

void MainMenu::newGameButtonClick()
{
  Gamani::getInstance().setFileToLoad("system.txt");
  Gamani::getInstance().setNextState(Gamani::GameState);
}

void MainMenu::loadButtonClick()
{
  Gamani::getInstance().setFileToLoad("save.txt");
  Gamani::getInstance().setNextState(Gamani::GameState);
}

void MainMenu::exitButtonClick()
{
  Gamani::getInstance().setNextState(Gamani::QuitState);
}
