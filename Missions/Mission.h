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
  void addMissionToAddOnComplete(Mission* mission) {addOnComplete_.push_back(mission);}
  void addMissionToRemoveOnComplete(Mission* mission) {removeOnComplete_.push_back(mission);}
  bool isVivible() {return visible_;}
  void setVisible(bool value) {visible_ = value;}
  void setFullDescr(vector<CString> fullDescr) {fullDescr_ = fullDescr;}
  const vector<CString>& getFullDescr() const {return fullDescr_;}
  void finalize();
private:
  list<Mission*> addOnComplete_;
  list<Mission*> removeOnComplete_;
  bool visible_;
  list<Task*> tasks_;
  CString description_;
  static int LastID;
  int id_;
  bool completed_;
  bool finalized_;

  vector<CString> fullDescr_;
};

class Task
{
public:
  enum TaskType {Approach, Land, Dock, MoveFrom, Orbit, Unknown};
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
  bool isCompletedNotCheck() {return completed_;}
  void setFullDescr(vector<CString> fullDescr) {fullDescr_ = fullDescr;}
  const vector<CString>& getFullDescr() const {return fullDescr_;}
  const list<Task*>& getDependsOn() const {return dependsOn_;}
private:
  void checkCompletion();
  bool checkApproachCompleted();
  bool checkLandCompleted();
  bool checkDockCompleted();
  bool checkOrbitCompleted();
  bool checkMoveFromCompleted();
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

  vector<CString> fullDescr_;
};
