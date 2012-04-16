#include "StdAfx.h"
#include "MissionDisplay.h"
#include "WTextBox.h"
#include "MissionManager.h"

MissionDisplay::MissionDisplay(TextPanel* textPanel):displayedMissionID_(-1),textPanel_(textPanel)
{

}

MissionDisplay::~MissionDisplay()
{

}

void MissionDisplay::init()
{
  visible_ = true;
  textPanel_->setVisible(false);
  //WTextBox* tb = new WTextBox();
  //tb->setDimensions(0.05, 0.05, 0.9, 0.9);
  //tb->addLine("aaa");
  //tb->addLine("bbb");
  //tb->addLine("ccc");
  //tb->addLine("ddd");
  //tb->addLine("eee");
  //tb->addLine("fff");
  //tb->addLine("ggg");
  //tb->addLine("hhh");
  //tb->addLine("iii");
  //tb->addLine("jjj");
  //tb->addLine("kkk");
  //tb->addLine("lll");
  //tb->addLine("mmm");
  //tb->addLine("nnn");
  //tb->addLine("ooo");
  //tb->addLine("ppp");
  //tb->addLine("qqq");
  //tb->addLine("rrr");
  //tb->addLine("sss");
  //tb->addLine("ttt");
  //addWidget(tb);
  
}

void MissionDisplay::render()
{
  WLayout::render();
}

void MissionDisplay::notifyChanged(int missionID, NotificationType type)
{
  switch (type) {
  case Added:
    addButtons(missionID);
    break;
  case Changed:
    updateData(missionID);
    break;
  case Removed:
    removeButtons(missionID);
    break;
  default:
    assert(0);
  }
}

void MissionDisplay::removeButtons(int missionID)
{
  Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
  if (missionID == displayedMissionID_) {
    clearTasks();
  }
  WButton* button = buttonsMap_[missionID];
  removeWidget(button);
  double top = button->getTop();
  for (auto itr = missionButtons_.begin(); itr != missionButtons_.end(); ++itr) {
    WButton* buttonItr = *itr;
    if (buttonItr->getTop() < top) {
      buttonItr->setDimensions(buttonItr->getLeft(), buttonItr->getTop() + 0.15, buttonItr->getWidth(), buttonItr->getHeight());
    }
  }
  missionButtons_.remove(button);
  delete button;
}

void MissionDisplay::updateData(int missionID)
{
  Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
  if (!mission->isVivible()) {
    return;
  }
  WButton* button = buttonsMap_[missionID];
  CString descr = mission->getDescription();
  if (mission->isCompleted()) {
    //descr = descr.append(" V");
    button->setHighlighted(true);
  } else {
    button->setHighlighted(false);
  }
  button->setLabel(descr);
  if (displayedMissionID_ == missionID) {
    displayedMissionID_ = -1;
    expandMission(missionID);
  }
  const list<Task*>& tasks = mission->getTasks();
  for (auto itr = tasks.begin(); itr != tasks.end(); ++itr) {
    Task* taskItr = *itr;
    if (!taskItr->isCompletedNotCheck()) {
      textPanel_->setText(taskItr->getFullDescr());
      textPanel_->setVisible(true);
      break;
    }
  }
}

void MissionDisplay::addButtons(int missionID)
{
  Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
  if (!mission->isVivible()) {
    return;
  }
  WButton* newButton = new WButton();
  double lastHeight = 0.85 + 0.15;
  if (missionButtons_.size() > 0) {
    WButton* lastButton = missionButtons_.back();
    lastHeight = lastButton->getTop();
  }
  newButton->setDimensions(0.02, lastHeight - 0.15, 0.28, 0.1);
  newButton->setLabel(mission->getDescription());
  newButton->setParam((void*)missionID);
  newButton->sigClickParam.connect(this, &MissionDisplay::expandMissionClick);
  addWidget(newButton);
  
  missionButtons_.push_back(newButton);
  buttonsMap_[missionID] = newButton;

  textPanel_->setText(mission->getFullDescr());
  textPanel_->setVisible(true);
}

void MissionDisplay::clearTasks()
{
  for (auto itr = tasks_.begin(); itr != tasks_.end(); ++itr) {
    WButton* taskText = *itr;
    removeWidget(taskText);
    delete taskText;
  }
  tasks_.clear();
}

void MissionDisplay::expandMission(int missionID)
{
  Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
  assert(mission);
  clearTasks();

  if (displayedMissionID_ == missionID) { //Just collapse tasks
    displayedMissionID_ = -1;
    return;
  }

  const list<Task*>& tasks = mission->getTasks();
  double height = 0.9;
  for (auto itr = tasks.begin(); itr != tasks.end(); ++itr) {
    Task* task = *itr;
    bool display = true;
    const list<Task*>& dependsOn = task->getDependsOn();
    for (auto itr = dependsOn.begin(); itr != dependsOn.end(); ++itr) {
      Task* taskItr = *itr;
      if (!taskItr->isCompletedNotCheck()) {
        display = false;
      }
    }
    if (!display) {
      continue;
    }
    WButton* taskText = new WButton();
    taskText->setDimensions(0.32, height, 0.65, 0.06);
    CString descr = task->getDescription();
    if (task->isCompleted()) {
      //descr = descr.append(" V");
      taskText->setHighlighted(true);
    } else {
      taskText->setHighlighted(false);
    }
    taskText->setLabel(descr);
    taskText->setParam((void*)task);
    taskText->sigClickParam.connect(this, &MissionDisplay::expandTask);
    addWidget(taskText);
    height -= 0.1;
    tasks_.push_back(taskText);
  }
  displayedMissionID_ = missionID;
}

void MissionDisplay::expandMissionClick(void* param)
{
  int missionID = (int) param;
  if (displayedMissionID_ != (int)param) {
    Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
    textPanel_->setText(mission->getFullDescr());
    textPanel_->setVisible(true);
  } else {
    textPanel_->setVisible(false);
  }
  expandMission(missionID);
}

void MissionDisplay::expandTask(void* param)
{
  Task* task = (Task*)param;
  textPanel_->setText(task->getFullDescr());
  textPanel_->setVisible(true);
}

