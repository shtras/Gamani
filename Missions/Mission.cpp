#include "StdAfx.h"
#include "Mission.h"

Mission::Mission():description_("")
{

}

Mission::~Mission()
{

}

void Mission::addTask(Task* task)
{
  tasks_.push_back(task);
}

Task::Task():target_(NULL), targetDist_(0), num_(0), type_(Unknown)
{

}

Task::~Task()
{

}

void Task::setDependsOn(Task* task)
{
  dependsOn_.push_back(task);
}
