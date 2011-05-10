#pragma once
#include "Star.h"

class StarSystem
{
public:
  StarSystem();
  ~StarSystem();

  void addStar(Star* star);
  const list<Star*>& getStars() const {return stars_;}
  vector<AstralBody*>* getAllObjects();
private:
  void fillVectorWithSattelites(AstralBody* body, vector<AstralBody*>& vec);
  list<Star*> stars_;
};

