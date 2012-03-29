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
  void skipTime(double time);
private:
  void advancePosition(AstralBody* from, AstralBody* to, double time);
  void advanceSublayer(AstralBody* start, Vector3 deltaCoord, Vector3 deltaVel);
  void fillVectorWithSattelites(AstralBody* body, vector<AstralBody*>& vec);
  list<Star*> stars_;
};

