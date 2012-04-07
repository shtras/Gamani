#include "StdAfx.h"
#include "MissionDisplay.h"
#include "WTextBox.h"
#include "MissionManager.h"

MissionDisplay::MissionDisplay():displayedMissionID_(-1)
{

}

MissionDisplay::~MissionDisplay()
{

}

void MissionDisplay::init()
{
  visible_ = true;
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
  default:
    assert(0);
  }
}

void MissionDisplay::updateData(int missionID)
{
  Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
  WButton* button = buttonsMap_[missionID];
  CString descr = mission->getDescription();
  if (mission->isCompleted()) {
    descr = descr.append(" V");
  }
  button->setLabel(descr);
  if (displayedMissionID_ == missionID) {
    displayedMissionID_ = -1;
    expandMission((void*)missionID);
  }
}

void MissionDisplay::addButtons(int missionID)
{
  Mission* mission = MissionManager::getInstance().getMissionByID(missionID);
  WButton* newButton = new WButton();
  double lastHeight = 0.85 + 0.15;
  if (missionButtons_.size() > 0) {
    WButton* lastButton = missionButtons_.back();
    lastHeight = lastButton->getTop();
  }
  newButton->setDimensions(0.02, lastHeight - 0.15, 0.28, 0.1);
  newButton->setLabel(mission->getDescription());
  newButton->setParam((void*)missionID);
  newButton->sigClickParam.connect(this, &MissionDisplay::expandMission);
  addWidget(newButton);
  
  missionButtons_.push_back(newButton);
  buttonsMap_[missionID] = newButton;
}

void MissionDisplay::clearTasks()
{
  for (auto itr = tasks_.begin(); itr != tasks_.end(); ++itr) {
    WText* taskText = *itr;
    removeWidget(taskText);
    delete taskText;
  }
  tasks_.clear();
}

void MissionDisplay::expandMission(void* param)
{
  int missionID = (int) param;
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
    WText* taskText = new WText();
    taskText->setDimensions(0.32, height, 0.68, 0.1);
    CString descr = task->getDescription();
    if (task->isCompleted()) {
      descr = descr.append(" V");
    }
    taskText->setText(descr);
    addWidget(taskText);
    height -= 0.1;
    tasks_.push_back(taskText);
  }
  displayedMissionID_ = missionID;
}
