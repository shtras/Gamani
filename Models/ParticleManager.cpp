#include "StdAfx.h"
#include "ParticleManager.h"
#include "Gamani.h"
#include "Renderer.h"
#include <algorithm>

ParticleManager::ParticleManager():smokeTex_(-1),lightTex_(-1),starTex_(-1)
{
}

ParticleManager::~ParticleManager()
{
  for (auto itr=particles_.begin(); itr != particles_.end(); ++itr) {
    Particle* particle = *itr;
    delete particle;
  }
}

void ParticleManager::updateCamPos()
{
  Camera& camera = Renderer::getInstance().getCamera();
  camPos_ = camera.getActualCoords();
  return;
  const double* dCamPos = camera.getPosition();
  Vector3 camPos = Vector3(-dCamPos[0], -dCamPos[1], /*dCamPos[2]*/0);
  double camDirAlpha = camera.getHeading();
  double camDirPhi = camera.getPitch();
  if (camDirPhi > 180) {
    camDirPhi -= 360.0;
  }
  if (camDirPhi < -180) {
    camDirPhi += 360.0;
  }
  double camZoom = camera.getZoom();
  double ralpha = DegToRad(camDirAlpha);
  double rphi = DegToRad(camDirPhi);
  double cx = sin(ralpha)*cos(rphi);
  double cy = cos(ralpha)*cos(rphi);
  double cz = sin(rphi);

  Vector3 camDir(cx, cy, cz);
  //   if (camDirPhi > 90 || camDirPhi < -90) {
  //     double temp = camDir[0];
  //     camDir[0] = -camDir[1];
  //     camDir[1] = -temp;
  //   }
  camDir.normalize();
  camDir *= 10/camZoom;
  camPos -= camDir;
  camPos *= 1/GLOBAL_MULT;
  camPos_ = camPos;
}

void ParticleManager::draw()
{
  if (particles_.size() == 0) {
    return;
  }
  updateCamPos();
  sortParticles();
  if (smokeTex_ == (GLuint)-1) {
    smokeTex_ = LoadBitmap22("Textures/smoketex.bmp");
  }
  if (lightTex_ == (GLuint)-1) {
    lightTex_ = LoadBitmap22("Textures/lighttex.bmp");
  }
  if (starTex_ == (GLuint)-1) {
    starTex_ = LoadBitmap22("Textures/startex.bmp");
  }
  glEnable(GL_TEXTURE_2D);
  for (auto itr=particles_.begin(); itr != particles_.end(); ++itr) {
    Particle* particle = *itr;
    switch (particle->getParticleType()) {
    case SmokeParticle:
      glBindTexture(GL_TEXTURE_2D, smokeTex_);
      break;
    case StarParticle:
      glBindTexture(GL_TEXTURE_2D, starTex_);
      break;
    case LightParticle:
      glBindTexture(GL_TEXTURE_2D, lightTex_);
      break;
    default:
      assert(0);
    }
    particle->render();
  }
  glDisable(GL_TEXTURE_2D);
}

void ParticleManager::updateLifetime()
{
  auto itr=particles_.begin();
  while (itr != particles_.end()) {
    Particle* particle = *itr;
    if (!particle->updateLifeTime()) {
      delete particle;
      itr = particles_.erase(itr);
    } else {
      ++itr;
    }
  }
}

void ParticleManager::reset()
{
  auto itr=particles_.begin();
  for (; itr != particles_.end(); ++itr) {
    Particle* particle = *itr;
    delete particle;
  }
  particles_.clear();
}

bool ParticleComp(Particle* p1, Particle* p2)
{
  const Vector3& camPos = Renderer::getInstance().getParticleManager()->getCamPos();
  double dist1 = (camPos - p1->getCoord()).getLength();
  double dist2 = (camPos - p2->getCoord()).getLength();
  return (dist1 > dist2);
}

void ParticleManager::addParticle(ParticleType type, Vector3& coord, Vector3& vel, int lifeTime, double size, Vector3& color)
{
  Particle* newPar = NULL;
  switch (type) {
  case SmokeParticle:
    newPar = new BouncingParticle(coord, vel, lifeTime, size);
    newPar->setParticleType(SmokeParticle);
    break;
  case LightParticle:
    newPar = new DynamicParticle(coord, vel, lifeTime, size, color);
    newPar->setParticleType(LightParticle);
    break;
  case StarParticle:
    newPar = new DynamicParticle(coord, vel, lifeTime, size);
    newPar->setParticleType(StarParticle);
    break;
  default:
    assert(0);
  }
   
  assert(newPar);
  particles_.push_back(newPar);
}

void ParticleManager::addParticle(ParticleType type, AstralBody* boundedTo, double dist, double angle, int lifeTime, double size, Vector3& color)
{
  Particle* newPar = NULL;
  switch (type) {
  case StarParticle:
    newPar = new BoundedParticle(boundedTo, dist, angle, lifeTime, size);
    newPar->setParticleType(StarParticle);
    break;
  case LightParticle:
    newPar = new BoundedParticle(boundedTo, dist, angle, lifeTime, size, color);
    newPar->setParticleType(LightParticle);
    break;
  default:
    assert(0);
  }

  assert(newPar);
  particles_.push_back(newPar);
}

void ParticleManager::sortParticles()
{
  std::sort(particles_.begin(), particles_.end(), ParticleComp);
}

//////////////////////////////////////////////////////////////////////////
Particle::Particle(int lifeTime):lifeTime_(lifeTime),currLife_(lifeTime), color_(Vector3(1,1,1))
{

}

Particle::Particle(int lifeTime, Vector3& color):lifeTime_(lifeTime), currLife_(lifeTime), color_(color)
{

}

Particle::~Particle()
{

}

bool Particle::updateLifeTime()
{
  coord_ *= 1.0e6;
  coord_ += vel_ * Gamani::getInstance().getSpeedReduce();
  coord_ *= 1.0/1.0e6;
  if (currLife_ == -1) {
    return true;
  }
  --currLife_;
  return currLife_ > 0;
}

void Particle::render()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  //Renderer::getInstance().getCamera().applyZoom();
  Vector3 coord = getRealCoordinates(coord_);



  double dist = coord.getLength();
  double zoom = Renderer::getInstance().getCamera().getZoom();
  double distMOne = 1/dist * 1e8 * 0.5;
  if (zoom > distMOne) {
    glScalef(distMOne, distMOne, distMOne);
  } else {
    Renderer::getInstance().getCamera().applyZoom();
  }




  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, coord[2]*GLOBAL_MULT);
  glDisable(GL_LIGHTING);


  const Vector3& camPos = Renderer::getInstance().getParticleManager()->getCamPos();
  Vector3 realCamPos = getRealCoordinates(camPos);
  Vector3 lookDir = camPos - coord_;
  lookDir.normalize();
  //cout << camDirAlpha << " " << camDirPhi << " " << camDir[2] << endl;

  Vector3 z = Vector3(0, 0, 1);

  Vector3 v1 = lookDir * z;
  v1.normalize();
  Vector3 v2 = lookDir * v1;
  v2.normalize();
  double size = size_ * currLife_ / (double)lifeTime_;

  //glBegin(GL_LINES);
  //glVertex3f(realCamPos[0]*GLOBAL_MULT, realCamPos[1]*GLOBAL_MULT, realCamPos[2]*GLOBAL_MULT);
  //glVertex3f(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, coord[2]*GLOBAL_MULT);
  //glEnd();
  if (particleType_ == ParticleManager::SmokeParticle) {
    glColor4f(0.1, 0.1, 0.1, currLife_ / (double)lifeTime_);
  } else if (particleType_ == ParticleManager::LightParticle) {
    glColor4f(color_[0], color_[1], color_[2], currLife_ / (double)lifeTime_);
  } else if (particleType_ == ParticleManager::StarParticle) {
    glColor4f(0.95, 1, 0.7, currLife_ / (double)lifeTime_);
  } else {
    assert(0);
  }
  //glEnable(GL_TEXTURE_2D);
  //glBindTexture(GL_TEXTURE_2D, Renderer::getInstance().particleManager_->getSmokeTex());

  glBegin(GL_POLYGON);
  glTexCoord2f(0, 0);
  glVertex3f(v1[0]*size*GLOBAL_MULT, v1[1]*size*GLOBAL_MULT, v1[2]*size*GLOBAL_MULT);
  glTexCoord2f(0, 1);
  glVertex3f(v2[0]*size*GLOBAL_MULT, v2[1]*size*GLOBAL_MULT, v2[2]*size*GLOBAL_MULT);
  glTexCoord2f(1, 1);
  glVertex3f(-v1[0]*size*GLOBAL_MULT, -v1[1]*size*GLOBAL_MULT, -v1[2]*size*GLOBAL_MULT);
  glTexCoord2f(1, 0);
  glVertex3f(-v2[0]*size*GLOBAL_MULT, -v2[1]*size*GLOBAL_MULT, -v2[2]*size*GLOBAL_MULT);
  glEnd();
  glEnable(GL_LIGHTING);
  //glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}
//////////////////////////////////////////////////////////////////////////

DynamicParticle::DynamicParticle(Vector3& coord, Vector3& vel, int lifeTime, double size, Vector3& color):Particle(lifeTime, color)
{
  type_ = Renderable::ParticleType;
  coord_ = coord;
  vel_ = vel;
  size_ = size;
}

DynamicParticle::DynamicParticle(Vector3& coord, Vector3& vel, int lifeTime, double size):Particle(lifeTime)
{
  type_ = Renderable::ParticleType;
  coord_ = coord;
  vel_ = vel;
  size_ = size;
}

DynamicParticle::~DynamicParticle()
{

}

//////////////////////////////////////////////////////////////////////////

BouncingParticle::BouncingParticle(Vector3& coord, Vector3& vel, int lifeTime, double size):
  DynamicParticle(coord, vel, lifeTime, size)
{

}

BouncingParticle::~BouncingParticle()
{

}

bool BouncingParticle::updateLifeTime()
{
  bool res = Particle::updateLifeTime();
  double r1 = 0.5 - rand() / (double)RAND_MAX;
  double r2 = 0.5 - rand() / (double)RAND_MAX;
  double r3 = 0.5 - rand() / (double)RAND_MAX;
  coord_ += Vector3(r1, r2, r3)*size_ * 0.3;
  return res;
}

//////////////////////////////////////////////////////////////////////////

BoundedParticle::BoundedParticle(AstralBody* boundedTo, double dist, double angle, int lifeTime, double size):Particle(lifeTime)
{
  boundedTo_ = boundedTo;
  dist_ = dist;
  angle_ = angle;
  size_ = size;
}

BoundedParticle::BoundedParticle(AstralBody* boundedTo, double dist, double angle, int lifeTime, double size, Vector3& color):Particle(lifeTime, color)
{
  boundedTo_ = boundedTo;
  dist_ = dist;
  angle_ = angle;
  size_ = size;
}

BoundedParticle::~BoundedParticle()
{

}

bool BoundedParticle::updateLifeTime()
{
  bool res = Particle::updateLifeTime();
  coord_ = boundedTo_->getCoord();
  vel_ = boundedTo_->getVelocity();
  double yaw = angle_ - boundedTo_->getYaw();
  yaw = DegToRad(yaw);
  coord_ += Vector3(dist_*sin(yaw), dist_*cos(yaw), 0);
  return res;
}
