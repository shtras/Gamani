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
  star->setRank(0);
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

void StarSystem::advanceSublayer(AstralBody* start, Vector3 deltaCoord, Vector3 deltaVel)
{
  start->setCoord(start->getCoord() + deltaCoord);
  start->setVelocity(start->getVelocity() + deltaVel);
  vector<AstralBody*> satellites = start->getSatellites();
  for (unsigned int i=0; i<satellites.size(); ++i) {
    advanceSublayer(satellites[i], deltaCoord, deltaVel);
  }
}

void StarSystem::advancePosition(AstralBody* from, AstralBody* to, double time)
{
  Vector3 r = (to->getCoord() - from->getCoord()) * 1e6;
  Vector3 v = (to->getVelocity() - from->getVelocity());
  double M = from->getMass() * 1e6;
  Vector3 h = r*v;
  Vector3 n = Vector3(0,1,0)*h;
  double G = 6.6725e-11;
  double myu = G*M;
  Vector3 e = (r*(v.dot(v) - myu/r.getLength()) - v*(r.dot(v)))*(1.0/myu);
  double E = v.dot(v)/2.0 - myu/r.getLength();
  double a = -myu/2.0/E;
  if (a < 0) {
    a = -a;
  }
  double elen = e.getLength();
  double b = sqrt(a*a*(1.0-e.dot(e)));
  double theta = h[2]/h.getLength();
  double tau = acos(n.dot(e) / n.getLength() / e.getLength()) * 180.0 / 3.14159265;
  if (e[1] < 0) {
    tau = 360-tau;
  }

  double eps = 1.0-(b*b/(a*a));
  if (eps > 0) {
    eps = sqrt(eps);
  } else {
    eps = 0;
  }
  double T = 2*PI*sqrt(a*a*a/myu); //period in seconds
  time -= ((long long)(time/T))*T;
  Vector3 fromCoord = from->getCoord();
  Vector3 toCoord = to->getCoord();
  Vector3 fromTo = toCoord - fromCoord;
  assert(time <= T && time > 0);
  double angle = fromTo.getAngle();
  double deltaAngle = 360.0*time/T;
  double newAngle = angle + deltaAngle;
  if (newAngle > 360.0) {
    newAngle -= 360.0;
  }
  assert(newAngle <= 360.0);
  double rangle = DegToRad(90 - newAngle);
  double len = fromTo.getLength();
  
  Vector3 newCoord = Vector3(fromCoord[0] + cos(rangle)*len, fromCoord[1] - sin(rangle)*len, 0);

  Vector3 fromVel = from->getVelocity();
  Vector3 toVel = to->getVelocity();
  Vector3 fromToVel = toVel - fromVel;
  double velAngle = fromToVel.getAngle();
  double velVal = fromToVel.getLength();
  double newVelAngle = velAngle + deltaAngle;
  double rvelAngle = DegToRad(90 - newVelAngle);
  Vector3 newVel = Vector3(fromVel[0] + cos(rvelAngle)*velVal, fromVel[1] - sin(rvelAngle)*velVal, 0);
  //////////////////////////////////////////////////////////////////////////
  //Here we assume that the orbit is circular (will work as a temporary assumption until I can come up with a way to calculate the new parameters for more complex cases)
  //PS yes, I'm lazy bastard
  //Vector3 newCoord = to->getCoord(); //???
  Vector3 oldCoord = to->getCoord();
  Vector3 deltaCoord = newCoord - oldCoord;
  //Vector3 newVel = to->getVelocity(); //???
  Vector3 oldVel = to->getVelocity();
  Vector3 deltaVel = newVel - oldVel;

  advanceSublayer(to, deltaCoord, deltaVel);
  vector<AstralBody*> satellites = to->getSatellites();
  for (unsigned int i=0; i<satellites.size(); ++i) {
    advancePosition(to, satellites[i], time);
  }
}

void StarSystem::skipTime(double time)
{
  assert(stars_.size() == 1);
  Star* star = stars_.front();
  vector<AstralBody*> satellites = star->getSatellites();
  for (unsigned int i=0; i<satellites.size(); ++i) {
    AstralBody* planet = satellites[i];
    advancePosition(star, planet, time);
  }
}
