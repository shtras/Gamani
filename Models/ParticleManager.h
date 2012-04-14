#pragma once
#include <list>
#include "Renderable.h"

class Particle;

class ParticleManager
{
public:
  enum ParticleType {SmokeParticle, LightParticle};
  ParticleManager();
  ~ParticleManager();

  void draw();
  void updateLifetime();
  void addParticle(ParticleType type, Vector3& coord, Vector3& vel, uint32_t lifeTime, double size);
  GLuint getSmokeTex() {return smokeTex_;}
  const Vector3& getCamPos() {return camPos_;}
private:
  void updateCamPos();
  void sortParticles();
  vector<Particle*> particles_;
  GLuint smokeTex_;
  GLuint lightTex_;
  Vector3 camPos_;
};

class Particle: public Renderable
{
public:
  Particle();
  virtual ~Particle();
  virtual void render() = 0;
  virtual bool updateLifeTime();

  virtual bool isStatic() {return false;}
  virtual bool isLanded() {return false;}
  virtual Vector3& getCoord() {return coord_;}
  virtual void setCoord(Vector3 coord) {coord_ = coord;}
  virtual CString getName() {return "";}
  virtual Vector3 getVelocity() {return vel_;}
  void setParticleType(ParticleManager::ParticleType type) {particleType_ = type;}
  ParticleManager::ParticleType getParticleType() {return particleType_;}
protected:
  int currLife_;
  int lifeTime_;
  Vector3 coord_;
  Vector3 vel_;
  double size_;
  ParticleManager::ParticleType particleType_;
};

class DynamicParticle: public Particle
{
public:
  DynamicParticle(Vector3& coord, Vector3& vel, uint32_t lifeTime, double size);
  ~DynamicParticle();
  virtual void render();
    
protected:
};

class BouncingParticle: public DynamicParticle
{
public:
  BouncingParticle(Vector3& coord, Vector3& vel, uint32_t lifeTime, double size);
  ~BouncingParticle();
  bool updateLifeTime();
protected:
};

//class BoundedParticle: public Particle
//{
//public:
//private:
//};
