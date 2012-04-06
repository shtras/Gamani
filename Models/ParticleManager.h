#pragma once
#include <list>
#include "Renderable.h"

class Particle;

class ParticleManager
{
public:
  ParticleManager();
  ~ParticleManager();

  void draw();
  void updateLifetime();
  void addParticle(Vector3& coord, Vector3& vel, uint32_t lifeTime, double size);
  GLuint getSmokeTex() {return smokeTex_;}
  const Vector3& getCamPos() {return camPos_;}
private:
  void updateCamPos();
  void sortParticles();
  vector<Particle*> particles_;
  GLuint smokeTex_;
  Vector3 camPos_;
};

class Particle: public Renderable
{
public:
  Particle(Vector3& coord, Vector3& vel, uint32_t lifeTime, double size);
  ~Particle();
  virtual void render();
  virtual bool isStatic() {return false;}
  virtual bool isLanded() {return false;}
  virtual Vector3& getCoord() {return coord_;}
  virtual void setCoord(Vector3 coord) {coord_ = coord;}
  virtual CString getName() {return "";}
  virtual Vector3 getVelocity() {return vel_;}
  bool updateLifeTime();
private:
  int currLife_;
  int lifeTime_;
  Vector3 coord_;
  Vector3 vel_;
  double size_;
};
