#pragma once
#include "AstralBody.h"

class Task;

class Mission
{
public:
  Mission();
  ~Mission();
private:
  list<Task*> tasks_;
  CString description_;
};

class Task
{
public:
  enum TaskType {Approach, Land, Dock, Orbit};
  Task();
  virtual ~Task();
private:
  list<Task*> dependsOn_;
  CString description_;
  TaskType type_;
  bool active_;
  AstralBody* target_;
  double targetDist_;
};
