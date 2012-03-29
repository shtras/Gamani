#pragma once
#include "AstralBody.h"

class Task;

class Mission
{
public:
  Mission();
  ~Mission();
  void addTask(Task* task);
  void setDescription(CString description) {description_ = description;}
private:
  list<Task*> tasks_;
  CString description_;
};

class Task
{
public:
  enum TaskType {Approach, Land, Dock, Orbit, Unknown};
  Task();
  virtual ~Task();
  void setDependsOn(Task* task);
  void setDescription(CString description) {description_ = description;}
  void setTarget(AstralBody* target) {target_ = target;}
  void setDist(double dist) {targetDist_ = dist;}
  void setNum(int num) {num_ = num;}
  void setType(TaskType type) {type_ = type;}
private:
  list<Task*> dependsOn_;
  CString description_;
  TaskType type_;
  bool active_;
  AstralBody* target_;
  double targetDist_;
  int num_;
};
