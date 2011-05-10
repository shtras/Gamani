#include "StdAfx.h"
#include "Camera.h"
#include "Renderer.h"
#include "Gamani.h"
#include "Renderable.h"

Camera::Camera():
heading_(180), pitch_(10),globalView_(false), lastX_(-1), lastY_(-1)
{
  position_[0] = 0;
  position_[1] = 0;
  position_[2] = 10;

  aspect_ = 1.0f;
  lastZoom_ = zoom_ = 50.0f;
}

Camera::~Camera()
{
}

void Camera::reset()
{
  position_[0] = 0;
  position_[1] = 0;
  //position_[2] = 150;

}

void Camera::setAspect(double aspect)
{
  aspect_ = aspect;
}

void Camera::move(double x, double y, double z)
{
  x/=zoom_;
  y/=zoom_;
  
  double heading = (-heading_)*3.14159265/180.0;
  double dirx = cos(heading);
  double diry = sin(heading);
  position_[0] += x*dirx;
  position_[1] += x*diry;

  position_[0] += y*diry;
  position_[1] += -y*dirx;

  position_[2] *= z;
}

void Camera::zoom(double val)
{
  if (val > 0) {
    zoom_ += zoom_ * 0.005f * val;
  } else {
    zoom_ += zoom_ * 0.005f * val;
  }
  if (zoom_ < 0.005) {
    zoom_ = 0.005;
  }
  //zoom_ += val;
}

void Camera::toggleGlobalView()
{
  if (globalView_) {
    globalView_ = false;
    zoom_ = lastZoom_;
  } else {
    globalView_ = true;
    lastZoom_ = zoom_;
    zoom_ = 1.0f;
  }
}

void Camera::rotate(double x, double y, double z)
{
  heading_ -= x;
  if (heading_ > 360) {
    heading_ -= 360;
  }
  if (heading_ < 0) {
    heading_ += 360;
  }
  pitch_ += y;
  if (pitch_ > 360) {
    pitch_ -= 360;
  }
  if (pitch_ < 0) {
    pitch_ += 360;
  }
}

void Camera::position()
{
  bool printData = Gamani::getInstance().getAuxPrints();
  glColor3f(1,1,1);
  if (printData) {
    Renderer::getInstance().textOut(-1,0.9,0, "Zoom: %0.2f Pos (%0.2f, %0.2f, %0.2f) Pitch: %0.2f Heading: %0.2f, Speed: %.3f StepLength: %.3f", 
      zoom_, position_[0], position_[1], position_[2], pitch_, heading_, Gamani::getInstance().getSpeed(), Gamani::getInstance().getSpeedReduce());

    Renderable* followed = Gamani::getInstance().getFollowedObject();
    string name = "Free camera";
    if (followed) {
      name = followed->getName();
      Renderer::getInstance().textOut(-1,0.85,0,"Pos: (%.2f, %.2f) Vel: (%.2f, %.2f)", followed->getCoord()[0], followed->getCoord()[1], followed->getVelocity()[0], followed->getVelocity()[1]);
    }
    Renderer::getInstance().textOut(-1,0.8,0, "%s", 
      name.c_str());

    Ship* ship = dynamic_cast<Ship*>(followed);
    if (ship) {
      Renderer::getInstance().textOut(-1,0.75,0, "Landed: %d Prog: %s Yaw: %0.2f, Target yaw: %0.2f, Target dist: %s", ship->isLanded(), ship->getProgName(ship->getProgram()).operator const char *(),
        ship->getYaw(), ship->getTgtAngle(), Renderer::getInstance().formatDistance(ship->getTgtDist()).operator const char *());
    }
  }

  if (Gamani::getInstance().isPaused()) {
    Renderer::getInstance().textOut(0, -0.9, 0, "PAUSE");
  }

  unsigned long long decim = Gamani::getInstance().getSeconds();
  int years = decim / 3153600000;
  decim -= years * 3153600000;
  int days = decim / 8640000;
  decim -= days * 8640000;
  int hours = decim / 360000;
  decim -= hours * 360000;
  int minutes = decim / 6000;
  decim -= minutes * 6000;
  int seconds = decim/100;
  decim -= seconds*100;

  Renderer::getInstance().textOut(-1,0.95,0, "%dY %dD %02d:%02d:%02d.%ld", years, days, hours, minutes, seconds, decim);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, aspect_, 0.1, 60000);
  gluLookAt(0, 0, 0, 
            0, 1, 0,
            0, 0, -1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glTranslatef(0, 0, -0.01);

  glRotatef(-180, 1, 0, 0);
  glTranslatef(0, -position_[2], 0);
  glRotatef(-pitch_, 1, 0, 0);
  //glTranslatef(0, position_[2], 0);
  glRotatef(180, 1, 0, 0);
  glRotatef(heading_, 0, 0, 1);
  //glTranslatef(0, 0, 10);

  //glTranslatef(10, 0, 0);

  //glTranslatef(position_[0], position_[1], position_[2]/zoom_);
}

void Camera::applyZoom()
{
  glScalef(zoom_, zoom_, zoom_);
}

void Camera::test()
{

}

void Camera::position(Vector3 pos)
{
  position_[0] = -pos[0]*GLOBAL_MULT;
  position_[1] = -pos[1]*GLOBAL_MULT;
}

void Camera::position(AstralBody* body)
{
  Vector3 newPos = body->getCoord();
  position_[0] = -newPos[0] * GLOBAL_MULT;
  position_[1] = -newPos[1] * GLOBAL_MULT;
  zoom_ = 1/ (body->getRadius() * GLOBAL_MULT);
}

void Camera::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  assert (message == WM_MOUSEMOVE);
  int xPos = GET_X_LPARAM(lParam);
  int yPos = GET_Y_LPARAM(lParam);
  if (lastX_ < 0 || lastY_ < 0) {
    assert (lastX_ == -1 && lastY_ == -1);
    lastX_ = xPos;
    lastY_ = yPos;
    return;
  }
  int dx = xPos - lastX_;
  int dy = yPos - lastY_;
  lastX_ = xPos;
  lastY_ = yPos;
  cout << dx << " " << dy << endl;
  rotate(dx*0.1, dy*0.1, 0);
}
