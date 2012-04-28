#pragma once
#include <list>
#include "Renderable.h"
#include "AstralBody.h"

class Particle;

class ParticleManager
{
public:
  enum ParticleType {SmokeParticle, LightParticle, StarParticle};
  ParticleManager();
  ~ParticleManager();

  void draw();
  void updateLifetime();
  void addParticle(ParticleType type, Vector3& coord, Vector3& vel, int lifeTime, double size, Vector3& color);
  void addParticle(ParticleType type, AstralBody* boundedTo, double dist, double angle, int lifeTime, double size, Vector3& color);
  GLuint getSmokeTex() {return smokeTex_;}
  const Vector3& getCamPos() {return camPos_;}
  void reset();
private:
  void updateCamPos();
  void sortParticles();
  vector<Particle*> particles_;
  GLuint smokeTex_;
  GLuint lightTex_;
  GLuint starTex_;
  Vector3 camPos_;
};

class Particle: public Renderable
{
public:
  Particle(int lifeTime, Vector3& color);
  Particle(int lifeTime);
  virtual ~Particle();
  virtual void render();
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
  Vector3 color_;
private:
  Particle();
};

class DynamicParticle: public Particle
{
public:
  DynamicParticle(Vector3& coord, Vector3& vel, int lifeTime, double size);
  DynamicParticle(Vector3& coord, Vector3& vel, int lifeTime, double size, Vector3& color);
  ~DynamicParticle();
protected:
};

class BouncingParticle: public DynamicParticle
{
public:
  BouncingParticle(Vector3& coord, Vector3& vel, int lifeTime, double size);
  ~BouncingParticle();
  bool updateLifeTime();
protected:
};

class BoundedParticle: public Particle
{
public:
  BoundedParticle(AstralBody* boundedTo, double dist, double angle, int lifeTime, double size);
  BoundedParticle(AstralBody* boundedTo, double dist, double angle, int lifeTime, double size, Vector3& color);
  ~BoundedParticle();
  virtual bool updateLifeTime();
private:
  AstralBody* boundedTo_;
  double dist_;
  double angle_;
  Vector3 color_;
};
