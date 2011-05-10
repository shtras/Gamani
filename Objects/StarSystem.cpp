#include "StdAfx.h"
#include "StarSystem.h"


StarSystem::StarSystem()
{
}


StarSystem::~StarSystem()
{
}

void StarSystem::addStar(Star* star)
{
  stars_.push_back(star);
}

vector<AstralBody*>* StarSystem::getAllObjects()
{
  vector<AstralBody*>& res = *new vector<AstralBody*>;

  fillVectorWithSattelites(stars_.front(), res);

  return &res;
}

void StarSystem::fillVectorWithSattelites(AstralBody* body, vector<AstralBody*>& vec)
{
  vector<AstralBody*> satellites = body->getSatellites();
  vec.push_back(body);
  for (uint32_t i=0; i<satellites.size(); ++i) {
    AstralBody* itr = satellites[i];
    fillVectorWithSattelites(itr, vec);
  }
}
