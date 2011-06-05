#include "StdAfx.h"
#include "World.h"
#include "DynamicBody.h"
#include "Planet.h"
#include "Renderer.h"
#include "Gamani.h"
#include "Autopilot.h"

#define OMP_NUM_THREADS 2

World::World():objects_(new vector<AstralBody*>),controlledShip_(NULL),followedObject_(NULL),currentSystem_(NULL)
{
}


World::~World()
{
  //list<Renderable*>::iterator itr = objects_->begin();
  //for (; itr != objects_->end(); ++itr) {
  //  Renderable* object = *itr;
  //  delete object;
  //}
  //delete objects_;
  unsigned int SZ = objects_->size();
  for (unsigned int i=0; i<SZ; ++i) {
    Renderable* object = (*objects_)[i];
    delete object;
  }
  delete objects_;
}

vector<AstralBody*>* World::getAllObjects()
{
  return objects_;
}

void World::switchControlledShip(Ship* ship)
{
  controlledShip_ = ship;
}

void World::addObject(Renderable* object)
{
  assert(0);
  //objects_->push_back(object);
}

bool World::removeObject(Renderable* object)
{
  assert(0);
  //objects_->remove(object);
  return true;
}

void World::snapshot()
{
  //list<Renderable*>::iterator itr = objects_->begin();
  //for (; itr != objects_->end(); ++itr) {
  unsigned int SZ = objects_->size();
  for (unsigned int i=0; i<SZ; ++i) {
    Renderable* ren = (*objects_)[i];//*itr;
    DynamicBody* pl = dynamic_cast<DynamicBody*>(ren);
    if (!pl) {
      continue;
    }
    pl->snapshot();
  }
}

void World::handlePressedKey(int key)
{
  bool pause = Gamani::getInstance().isPaused();
  switch (key) {
  case 'A':
    if (controlledShip_ && !pause) {
      controlledShip_->yawLeft();
    }
    break;
  case 'D':
    if (controlledShip_ && !pause) {
      controlledShip_->yawRight();
    }
    break;
  case 'W':
    if (controlledShip_ && !pause) {
      controlledShip_->accelerate();
    }
    break;
  case 'S':
    if (controlledShip_ && !pause) {
      controlledShip_->back();
    }
    break;
  case 'K':
    if (controlledShip_) {
      controlledShip_->setAutopilotTo(Autopilot::KillRot);
    }
    break;
  case 0x30:
    if (controlledShip_) {
      controlledShip_->setAutopilotTo(Autopilot::Approach);
    }
    break;
  case 'R':
    if (controlledShip_) {
      controlledShip_->setAutopilotTo(Autopilot::Rotate);
    }
    break;
  case 'L':
    if (controlledShip_) {
      controlledShip_->setAutopilotTo(Autopilot::Launch);
    }
    break;
  case 'N':
    scrollFollowedObject();
    if (followedObject_) {
      Renderer::getInstance().getCamera().position(followedObject_);
    }
    break;
  case 'F':
    followedObject_ = NULL;
    break;
  case 'Z':
    selectShip();
    break;
  case 'Q':
    controlledShip_->steerLeft();
    //controlledShip_->scrollGravityRef();
    break;
  case 'E':
    controlledShip_->steerRight();
    //controlledShip_->toggleAutoRef();
    break;
  case 0x39:
    controlledShip_->undock();
    break;
  }
}

void World::selectShip()
{
  followedObject_ = controlledShip_;
  if (controlledShip_) {
    Renderer::getInstance().getCamera().position(followedObject_);
  }
}

void World::updatePosition(Renderable* obj)
{
  if (obj->isStatic()) {
    return;
  }

  assert(!obj->isStatic());
  DynamicBody* dynObj = (DynamicBody*)obj;
  
  if (dynObj->getType() == Renderable::ShipType) {
    Ship* ship = (Ship*)dynObj;
    if (ship->isUndocking()) {
      ship->checkUndocking();
    } else if (ship->isDocked()) {
      ship->setDockedCoord();
      //ship->setCoord(ship->getDockedCoord());
      return;
    }
  }

  if (dynObj->isLanded()) {
    assert(dynObj->getType() == Renderable::ShipType);
    Ship* ship = (Ship*)dynObj;
    if (!ship->isLaunching()) {
      dynObj->setCoord(dynObj->getLandedCoords());
      dynObj->setVelocity(dynObj->getRefBody()->getVelocity());
    }
  } else {
    Vector3 coord = dynObj->getCoord();
    coord *= 1.0e6;
    coord += dynObj->getVelocity() * (Gamani::getInstance().getSpeedReduce());
    coord *= 1.0/1.0e6;
    dynObj->setCoord(coord);
    double newYaw = dynObj->getYaw() + dynObj->getYawVel();
    if (newYaw >= 360) {
      newYaw -= 360.0;
    }
    if (newYaw < 0) {
      newYaw += 360.0;
    }
    dynObj->setYaw(newYaw);

    if (followedObject_) {
      Renderer::getInstance().getCamera().position(followedObject_->getCoord());
    }
  }
}

void World::interactGravity(Renderable* from, Renderable* to)
{
  if (from == to) {
    return;
  }
  if (to->isStatic()) {
    return;
  }
  //if (!obj1->isStatic() && !obj2->isStatic()) {
  //  return;
  //}

  AstralBody* bodyFrom = (AstralBody*)from; //dynamic_cast<AstralBody*>(from);
  AstralBody* bodyTo = (AstralBody*)to; //dynamic_cast<AstralBody*>(to);
  assert(bodyFrom && bodyTo);

  //if (bodyFrom->getName() == CString("Earth")) {
  //  AstralBody* moon = bodyFrom->getSatellites()[0];
  //  Vector3 dir = moon->getCoord() - bodyFrom->getCoord();
  //  double dist = dir.getLength();
  //  int a = 0;
  //}

  Vector3 coordFrom = bodyFrom->getCoord();
  Vector3 coordTo = bodyTo->getCoord();
  double massFrom = bodyFrom->getMass() * 1e6; //kg
  double massTo = bodyTo->getMass() * 1e6; //kg
  //G*m*m/r^2
  static double G = 6.6725e-11;
  double distSquare = (coordFrom[0]-coordTo[0])*(coordFrom[0]-coordTo[0])+(coordFrom[1]-coordTo[1])*(coordFrom[1]-coordTo[1])+(coordFrom[2]-coordTo[2])*(coordFrom[2]-coordTo[2]);
  distSquare *= 1e6*1e6; //meters^2
  double speedReduce = Gamani::getInstance().getSpeedReduce();
  double force = G*massFrom*massTo/distSquare * speedReduce * speedReduce; //kg^2/m^2*M*m^2/kg^2 = N
  //if (!body2->isStatic()) {
  Vector3 velocity = bodyTo->getVelocity() * (speedReduce);
  //velocity *= 1000.0; //m/s
  Vector3 dir = coordFrom - coordTo;
  dir.normalize();
  //f = m*a; a=f/m; a=dv
  double dVelocity = force/massTo;
  //velocity += Vector3(dir[0]*dVelocity, dir[1]*dVelocity, dir[2]*dVelocity);
  velocity += dir*dVelocity;
  bodyTo->setVelocity(velocity * (1/speedReduce)/**(1.0/1000.0f)*/);
  //} 
  //if (!body1->isStatic()) {
  //  Vector3 velocity = body1->getVelocity();
  // // velocity *= 1000.0; //m/s
  //  Vector3 dir = coord2 - coord1;
  //  dir.normalize();
  //  double dVelocity = force/mass1;
  //  velocity += Vector3(dir[0]*dVelocity, dir[1]*dVelocity, dir[2]*dVelocity);
  //  body1->setVelocity(velocity/**(1.0/1000.0f)*/);
  //}
}

void World::scrollFollowedObject()
{
  if (!followedObject_) {
    if (objects_->size() > 0) {
      followedObject_ = (*objects_)[0];//*objects_->begin();
    }
    return;
  }
  //list<Renderable*>::iterator itr = objects_->begin();
  //for (; itr != objects_->end(); ++itr) {
  unsigned int SZ = objects_->size();
  for (unsigned int i=0; i<SZ; ++i) {
    //if (followedObject_ == *itr) {
    //  ++itr;
    //  if (itr == objects_->end()) {
    //    followedObject_ = NULL;
    //    Renderer::getInstance().getCamera().reset();
    //    return;
    //  }
    //  followedObject_ = *itr;
    //  return;
    //}
    if (followedObject_ == (*objects_)[i]) {
      if (i == objects_->size()-1) {
        followedObject_ = NULL;
        Renderer::getInstance().getCamera().reset();
        return;
      }
      followedObject_ = (*objects_)[i+1];
      return;
    }
  }
}

void World::interactCollision(Renderable* obj1, Renderable* obj2)
{
  if (obj1->isStatic() && obj2->isStatic()) {
    return;
  }
  AstralBody* body1 = (AstralBody*)obj1;
  AstralBody* body2 = (AstralBody*)obj2;
  if (!checkCollision(body1, body2)) {
    return;
  }
  if (body1->isStatic()) {
    body2->collideWith(body1);
  } else {
    body1->collideWith(body2);
  }
}

bool World::checkCollision(AstralBody* body1, AstralBody* body2)
{
  if (!body1 || !body2) {
    return false;
  }
  if (body1->isLanded() || body2->isLanded()) {
    return false;
  }
  Vector3 coord1 = body1->getCoord();
  Vector3 coord2 = body2->getCoord();
  double dist = (coord1 - coord2).getLength();
  double minDist = body1->getRadius() + body2->getRadius();
  if (dist <= minDist) {
    return true;
  }
  return false;
}

//void World::interactionStep()
//{
//  //list<Renderable*>::iterator outItr;
//  //for (outItr = objects_->begin(); outItr != objects_->end(); ++outItr) {
//  vector<Renderable*>& objs = *objects_;
//  int SZ = objects_->size();
//  for (int i=0; i<SZ; ++i) {
//    //list<Renderable*>::iterator inItr = outItr;
//    //int j = i+1;
//    //++inItr;
//    Renderable* obj1 = /**outItr*/objs[i];
//    if (obj1->isLanded()) {
//      continue;
//    }
//    for (int j = i+1; j < SZ; ++j) {
//      //obj1 = (*objects_)[i];
//      Renderable* obj2 = /**inItr*/objs[j];
//      if (obj2->isLanded()) {
//        continue;
//      }
//      interactGravity(obj1, obj2);
//      interactCollision(obj1, obj2);
//    }
//    updatePosition(obj1);
//  }
//}

void World::setStarSystem(StarSystem* system)
{
  currentSystem_ = system;
  for (uint32_t i=0; i<objects_->size(); ++i) {
    Renderable* itr = (*objects_)[i];
    delete itr;
  }
  delete objects_;

  objects_ = currentSystem_->getAllObjects();

}

void World::addFreeObject(AstralBody* object)
{
  freeObjects_.push_back(object);
  objects_->push_back(object);
}

void World::interactionStep()
{
  const list<Star*>& stars = currentSystem_->getStars();
  if (stars.size() != 1) {
    //Unimplemented
    assert(0);
  }
  Star* mainStar = *stars.begin();
  gravitySubLevelsInteraction(mainStar, mainStar->getSatellites(), true);
  for (uint32_t i=0; i<objects_->size(); ++i) {
    AstralBody* itr = (*objects_)[i];

    if (itr->getType() == Renderable::ShipType) {
      Ship* ship = (Ship*)itr;
      ship->autopilotStep();
    }

    itr->rotationStep();
    updatePosition(itr);
    for (uint32_t j=0; j<freeObjects_.size(); ++j) {
      AstralBody* freeItr = freeObjects_[j];
      interactGravity(itr, freeItr);
    }
  }

  for (uint32_t i=0; i<freeObjects_.size(); ++i) {
    AstralBody* itr = freeObjects_[i];
    for (uint32_t j=0; j<objects_->size(); ++j) {
      AstralBody* innerItr = (*objects_)[j];
      if (innerItr == itr) {
        continue;
      }
      interactCollision(itr, innerItr);
    }
  }

  //for (uint32_t i=0; i<freeObjects_.size(); ++i) {
  //  AstralBody* itr = freeObjects_[i];
  //  //updatePosition(itr);
  //  for (uint32_t j=0; j<freeObjects_.size(); ++j) {
  //    AstralBody* innerItr = freeObjects_[j];
  //    if (innerItr == itr) {
  //      continue;
  //    }
  //    interactGravity(itr, innerItr);
  //  }
  //}
}

void World::gravityWithinLevelInteraction(AstralBody* head, vector<AstralBody*>& level)
{
  if (level.size() == 0) {
    assert(0);
  }
  unsigned int SZ = level.size();
  for (uint32_t i=0; i<SZ; ++i) {
    AstralBody* itr = level[i];
    if (itr != head) {
      interactGravity(head, itr);
    }
  }
}

void World::gravitySubLevelsInteraction(AstralBody* head, vector<AstralBody*>& level, bool mutual)
{
  if (level.size() == 0) {
    return;
  }
  unsigned int SZ = level.size();
  for (uint32_t i=0; i<SZ; ++i) {
    AstralBody* itr = level[i];
    gravityWithinLevelInteraction(itr, level);
    interactGravity(head, itr);
    if (mutual) {
      interactGravity(itr, head);
    }
    gravitySubLevelsInteraction(head, itr->getSatellites(), false);
    gravitySubLevelsInteraction(itr, itr->getSatellites(), true);
  }
}
