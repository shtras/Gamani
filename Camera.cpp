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
  //if (Gamani::getInstance().getNextState() == Gamani::MenuState) {
  //  return;
  //}
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
  if (Gamani::getInstance().getNextState() == Gamani::MenuState) {
    return;
  }
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

double Camera::getHeading() const
{
  if (Gamani::getInstance().rotateCameraWithObject() && Gamani::getInstance().getWorld()->getFollowedObject()) {
    return heading_ - Gamani::getInstance().getWorld()->getFollowedObject()->getYaw();
  } else {
    return heading_;
  }
}

void Camera::position()
{
  double testHeading = heading_;
  if (Gamani::getInstance().rotateCameraWithObject() && Gamani::getInstance().getWorld()->getFollowedObject()) {
    testHeading -= Gamani::getInstance().getWorld()->getFollowedObject()->getYaw();
  }

  bool printData = Gamani::getInstance().getAuxPrints() && (Gamani::getInstance().getNextState() == Gamani::GameState);
  glColor3f(1,1,1);
  if (printData) {
    Renderer::getInstance().textOut(0 ,0.95,0, "Pitch: %0.2f Heading: %0.2f, Speed: %.3f StepLength: %.3f",
      pitch_, testHeading, Gamani::getInstance().getSpeed(), Gamani::getInstance().getSpeedReduce());

    //Renderer::getInstance().textOut(0 ,0.75,0, "%0.3f %0.3f", Gamani::getInstance().getFollowedObject()->getVelocity()[0],Gamani::getInstance().getFollowedObject()->getVelocity()[1]);

    //Renderable* followed = Gamani::getInstance().getFollowedObject();
    //string name = "Free camera";
    //if (followed) {
    //  name = followed->getName();
    //  Renderer::getInstance().textOut(-1,0.75,0,"Pos: (%.2f, %.2f) Vel: (%.2f, %.2f)", followed->getCoord()[0], followed->getCoord()[1], followed->getVelocity()[0], followed->getVelocity()[1]);
    //}
    //Renderer::getInstance().textOut(-1,0.7,0, "%s", name.c_str());

    //if (followed && followed->getType() == Renderable::ShipType) {
    //  Ship* ship = static_cast<Ship*>(followed);
    //  Renderer::getInstance().textOut(-1,0.65,0, "Landed: %d Docked: %s Yaw: %0.2f Program: %s", ship->isLanded(), (ship->isDocked()? "Docked" : (ship->dockedTo_?"Undocking":"No")), 
    //    ship->getYaw(), ship->getCurrProgName());
    //}
    if (Gamani::getInstance().isPaused()) {
      Renderer::getInstance().textOut(0, -0.9, 0, "PAUSE");
    }
  }

  if (Gamani::getInstance().getNextState() == Gamani::MenuState) {
    Renderer::getInstance().textOut(0.7, -0.95, 0, "Gamani %s", Gamani::getInstance().getVersion());
    pitch_ = 20;
    heading_ = 310;
    zoom_ = 6000;
  }

  unsigned long long decim = Gamani::getInstance().getSeconds();
  int years = decim / 3153600000LL;
  decim -= years * 3153600000LL;
  int days = decim / 8640000LL;
  decim -= days * 8640000LL;
  int hours = decim / 360000LL;
  decim -= hours * 360000LL;
  int minutes = decim / 6000LL;
  decim -= minutes * 6000LL;
  int seconds = decim/100LL;
  decim -= seconds*100LL;

  if (printData) {
    Renderer::getInstance().textOut(-0.4,0.95,0, "%dY %dD %02d:%02d:%02d.%02ld", years, days, hours, minutes, seconds, decim);
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, aspect_, 0.1, 1e10);
  gluLookAt(0, 0, 0, 
            0, 1, 0,
            0, 0, -1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glTranslatef(0, 0, -0.01);

  glRotatef(-180, 1, 0, 0);
  //assert(fabs (position_[2] - 10) < 0.000001);
  glTranslatef(0, -/*position_[2]*/10, 0);
  glRotatef(-pitch_, 1, 0, 0);
  //glTranslatef(0, position_[2], 0);
  glRotatef(180, 1, 0, 0);
  glRotatef(testHeading, 0, 0, 1);
  //glTranslatef(0, 0, 10);

  //glTranslatef(10, 0, 0);

  //glTranslatef(position_[0], position_[1], position_[2]/zoom_);
  calcRealCoords();
}

void Camera::calcRealCoords()
{
  const double* dCamPos = position_;
  Vector3 camPos = Vector3(-dCamPos[0], -dCamPos[1], -dCamPos[2]);
  double camDirAlpha = heading_;
  double camDirPhi = pitch_;
  if (camDirPhi > 180) {
    camDirPhi -= 360.0;
  }
  if (camDirPhi < -180) {
    camDirPhi += 360.0;
  }
  double camZoom = zoom_;
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
  actualCoords_ = camPos;
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
  position_[2] = -pos[2]*GLOBAL_MULT;
}

void Camera::position(AstralBody* body)
{
  Vector3 newPos = body->getCoord();
  position_[0] = -newPos[0] * GLOBAL_MULT;
  position_[1] = -newPos[1] * GLOBAL_MULT;
  position_[2] = -newPos[2] * GLOBAL_MULT;
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
  //cout << dx << " " << dy << endl;
  rotate(dx*0.1, dy*0.1, 0);
}
