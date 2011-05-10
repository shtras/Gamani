#pragma once
#include "Renderable.h"
#include "Ship.h"
#include "StarSystem.h"

class World
{
public:
  World();
  ~World();
  vector<AstralBody*>* getAllObjects();
  void interactionStep();
  void addObject(Renderable* object);
  void addFreeObject(AstralBody* object);
  bool removeObject(Renderable* object);
  void switchControlledShip(Ship* ship);
  void handlePressedKey(int key);
  void snapshot();
  void switchFollowedObject(AstralBody* object) {followedObject_ = object;}
  AstralBody* getFollowedObject() {return followedObject_;}
  void setStarSystem(StarSystem* system);
  const StarSystem* getCurrentSystem() const {return currentSystem_;}
private:
  void interactGravity(Renderable* obj1, Renderable* obj2);
  void updatePosition(Renderable* obj);
  bool checkCollision(AstralBody* body1, AstralBody* body2);
  void interactCollision(Renderable* obj1, Renderable* obj2);
  void scrollFollowedObject();
  void gravityWithinLevelInteraction(AstralBody* head, vector<AstralBody*>& level);
  void gravitySubLevelsInteraction(AstralBody* head, vector<AstralBody*>& level, bool mutual);
  vector<AstralBody*>* objects_;
  vector<AstralBody*> freeObjects_;
  Ship* controlledShip_;
  AstralBody* followedObject_;
  StarSystem* currentSystem_;
};

