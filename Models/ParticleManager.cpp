#include "StdAfx.h"
#include "ParticleManager.h"
#include "Gamani.h"
#include "Renderer.h"
#include <algorithm>

ParticleManager::ParticleManager():smokeTex_(-1)
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
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, smokeTex_);
  for (auto itr=particles_.begin(); itr != particles_.end(); ++itr) {
    Particle* particle = *itr;
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

bool ParticleComp(Particle* p1, Particle* p2)
{
  const Vector3& camPos = Renderer::getInstance().getParticleManager()->getCamPos();
  double dist1 = (camPos - p1->getCoord()).getLength();
  double dist2 = (camPos - p2->getCoord()).getLength();
  return (dist1 > dist2);
}

void ParticleManager::addParticle(Vector3& coord, Vector3& vel, uint32_t lifeTime, double size)
{
  Particle* newPar = new Particle(coord, vel, lifeTime, size);
  particles_.push_back(newPar);
}

void ParticleManager::sortParticles()
{
  std::sort(particles_.begin(), particles_.end(), ParticleComp);
}

//////////////////////////////////////////////////////////////////////////

Particle::Particle(Vector3& coord, Vector3& vel, uint32_t lifeTime, double size):
currLife_(lifeTime), coord_(coord), vel_(vel),size_(size),lifeTime_(lifeTime),rot_(0)
{
  type_ = Renderable::ParticleType;
}

Particle::~Particle()
{

}

bool Particle::updateLifeTime()
{
  rot_ += 0.5 - rand() / (double)RAND_MAX;
  if (rot_ > 360) {
    rot_ = 0;
  }
  coord_ *= 1.0e6;
  coord_ += vel_ * Gamani::getInstance().getSpeedReduce();
  coord_ *= 1.0/1.0e6;
  --currLife_;
  return currLife_ > 0;
}

void Particle::render()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  Renderer::getInstance().getCamera().applyZoom();
  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  Vector3 coord = getRealCoordinates(coord_);
  glRotatef(rot_, 0, 1, 0);

  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);
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
  glColor4f(0.2, 0.2, 0.2, currLife_ / (double)lifeTime_);
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
