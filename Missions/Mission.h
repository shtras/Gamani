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
  const int getID() const {return id_;}
  CString getDescription() {return description_;}
  const list<Task*>& getTasks() const {return tasks_;}
  void checkCompleted();
  bool isCompleted();
private:
  list<Task*> tasks_;
  CString description_;
  static int LastID;
  int id_;
  bool completed_;
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
  const int getID() const {return id_;}
  CString getDescription() {return description_;}
  bool isCompleted();
private:
  void checkCompletion();
  bool checkApproachCompleted();
  bool checkLandCompleted();
  bool checkDockCompleted();
  bool checkOrbitCompleted();
  list<Task*> dependsOn_;
  CString description_;
  TaskType type_;
  bool active_;
  AstralBody* target_;
  double targetDist_;
  int num_;
  static int LastID;
  int id_;
  bool completed_;
};
