#include "StdAfx.h"
#include "Ship.h"
#include "Renderer.h"
#include "Gamani.h"
#include "HUD.h"
#include "APProgram.h"


Ship::Ship():autopilot_(NULL)
{
  mass_ = 0.00005;
  yaw_ = 90;
  yawPower_ = 0.05;
  maxYawVel_ = 10;
  maxSpeed_ = 500;
  marchPower_ = 5;
  radius_ = 0.05;
//   currProg_ = NoProgram;
//   targetAngle_ = -100;
  color_[0] = 1;
  color_[1] = 0.5;
  color_[2] = 0;
  landed_ = false;
  docked_ = false;
  landedOn_ = NULL;
  dockedTo_ = NULL;
  launchCount_ = 0;
  //gravityRef_ = NULL;
  hud_ = NULL;
  test1_ = NULL;
  test2_ = NULL;
  //manualRef_ = false;
  refIdx_ = -1;
//   targetDist_ = 0;
//   startAngle_ = -100;
//   rotateRight_ = false;
//   rotatePhase1_ = true;
  type_ = ShipType;
  autopilot_ = new Autopilot(this);
}

Ship::~Ship()
{
  delete hud_;
  delete autopilot_;
}

void Ship::render()
{
  hud_->updateData();

  DynamicBody::render();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  Renderer::getInstance().getCamera().applyZoom();
  glColor3f(color_[0], color_[1], color_[2]);

  //glTranslatef(coord_[0]*GLOBAL_MULT, coord_[1]*GLOBAL_MULT, coord_[2]*GLOBAL_MULT);
  Vector3 coord = getRealCoordinates(coord_);
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, 0/*coord[2]*GLOBAL_MULT*/);

  glRotatef(-90, 1, 0, 0);
  glRotatef(180-yaw_, 0, 1, 0);
  //glTranslatef(0, 0, -radius_*GLOBAL_MULT/2.0);

  const Model* model = getModel();
  if (model) {
    glScalef(radius_*GLOBAL_MULT, radius_*GLOBAL_MULT, radius_*GLOBAL_MULT);
    model->draw();
  } else {
    glutSolidCone(radius_*GLOBAL_MULT/4.0f, radius_*GLOBAL_MULT, 10, 5);
  }


  //renderPort();

  drawName();

  glPopMatrix();


  //if (Gamani::getInstance().getAuxAxes()) {
  //  glPushMatrix();
  //  Renderer::getInstance().getCamera().applyZoom();

  //  glDisable(GL_LIGHTING);
  //  Vector3 relSpd = velocity_ - gravityRef_->getVelocity();
  //  double spd = relSpd.getLength();
  //  relSpd.normalize();
  //  relSpd *= radius_ * 0.5 * spd/10000.0 / 1000.0;
  //  glColor3f(0, 0.2, 0.9);
  //  glBegin(GL_LINES);
  //    glVertex3f(0,0,radius_*GLOBAL_MULT);
  //    glVertex3f(relSpd[0], relSpd[1], radius_*GLOBAL_MULT);
  //  glEnd();

  //  Vector3 dir = gravityRef_->getCoord() - coord_;
  //  double dist = dir.getLength();
  //  dir.normalize();
  //  dir *= radius_ * 0.5 * sqrt(dist/10) / 1000.0;
  //  glColor3f(0, 0.9, 0.2);
  //  glBegin(GL_LINES);
  //  glVertex3f(0,0,radius_*GLOBAL_MULT);
  //  glVertex3f(dir[0], dir[1], radius_*GLOBAL_MULT);
  //  glEnd();

  //  Vector3 sDir = Vector3(sin(yaw_*3.14159265/180.0), -cos(yaw_*3.14159265/180.0), 0);
  //  sDir *= radius_ * (1/10000.0);
  //  glColor3f(0.7, 0.9, 0.7);
  //  glBegin(GL_LINES);
  //  glVertex3f(0,0,radius_*GLOBAL_MULT);
  //  glVertex3f(sDir[0], sDir[1], radius_*GLOBAL_MULT);
  //  glEnd();

  //  glEnable(GL_LIGHTING);
  //  glPopMatrix();
  //}

  //testNavCom();
  //testNavCom1();
}

//void Ship::updateGravityRef()
//{
//  if (manualRef_) {
//    return;
//  }
//  vector<AstralBody*>& objects = *Gamani::getInstance().getWorld()->getAllObjects();
//  AstralBody* newRef = NULL;
//  double maxGrav = 0;
//  Vector3 coordTo = coord_;
//
//  for (uint32_t i=0; i<objects.size(); ++i) {
//    AstralBody* obj = objects[i];
//    if (obj == this) {
//      continue;
//    }
//
//    //if (obj->getName() == CString("Earth") || obj->getName() == CString("Shipyard")) {
//    //  int aaa = 0;
//    //}
//
//    Vector3 coordFrom = obj->getCoord();
//    double massFrom = obj->getMass() * 1e6; //kg
//    double massTo = getMass() * 1e6; //kg
//    //G*m*m/r^2
//    static double G = 6.6725e-11;
//    double distSquare = (coordFrom[0]-coordTo[0])*(coordFrom[0]-coordTo[0])+(coordFrom[1]-coordTo[1])*(coordFrom[1]-coordTo[1])+(coordFrom[2]-coordTo[2])*(coordFrom[2]-coordTo[2]);
//    distSquare *= 1e6*1e6; //meters^2
//    double force = G*massFrom*massTo/distSquare; //kg^2/m^2*M*m^2/kg^2 = N
//    if (force > maxGrav) {
//      maxGrav = force;
//      newRef = obj;
//    }
//  }
//  if (newRef) {
//    gravityRef_ = newRef;
//  } else {
//    assert(0);
//  }
//}

//void Ship::toggleAutoRef()
//{
//  manualRef_ = false;
//  refIdx_ = -1;
//}

//void Ship::scrollGravityRef()
//{
//  manualRef_ = true;
//  vector<AstralBody*>& objects = *Gamani::getInstance().getWorld()->getAllObjects();
//  if (++refIdx_ >= objects.size()) {
//    refIdx_ = 0;
//  }
//  gravityRef_ = objects[refIdx_];
//}

//void Ship::testNavCom()
//{
//  if (!gravityRef_) {
//    return;
//  }
//  updateGravityRef();
//  Renderer::getInstance().requestViewPort(0,0, 300, 300);
//  //Renderer::getInstance().requestViewPort(Renderer::getInstance().getWidth() - 300 ,0, 300, 300);
//
//  Renderer::getInstance().textOut(0, 0.8, 0, "%s", gravityRef_->getName().operator const char *());
//  double distance = sqrt((coord_[0]-gravityRef_->getCoord()[0])*(coord_[0]-gravityRef_->getCoord()[0]) + (coord_[1]-gravityRef_->getCoord()[1])*(coord_[1]-gravityRef_->getCoord()[1]));
//  Vector3 relSpd = velocity_ - gravityRef_->getVelocity();
//  Renderer::getInstance().textOut(0, 0.7, 0, "Dist: %.1f RelSpd: (%.1f, %.1f)", distance, relSpd[0], relSpd[1]);
//
//  glDisable(GL_LIGHTING);
//
//  glMatrixMode(GL_PROJECTION);
//  glPushMatrix();
//  glLoadIdentity();
//  glMatrixMode(GL_MODELVIEW);
//  glPushMatrix();
//  glLoadIdentity();
//
//  glBegin(GL_LINE_LOOP);
//  glVertex3f(-0.9, -0.9, 0);
//  glVertex3f(0.9, -0.9, 0);
//  glVertex3f(0.9, 0.9, 0);
//  glVertex3f(-0.9, 0.9, 0);
//  glEnd();
//
//  glBegin(GL_LINE_LOOP);
//  glVertex3f(0.7, 0.9, 0);
//  glVertex3f(0.9, 0.9, 0);
//  glVertex3f(0.9, 0.7, 0);
//  glVertex3f(0.7, 0.7, 0);
//  glEnd();
//
//
//  Vector3 direction = coord_ - gravityRef_->getCoord();
//  direction.normalize();
//  direction *= 0.1;
//  glBegin(GL_LINES);
//  glVertex3f(0.8, 0.8, 0);
//  glVertex3f(0.8 - direction[0], 0.8 + direction[1], 0);
//  glEnd();
//
//
//  //glLoadIdentity();
//  //vector<Vector3> points;
//  //points.push_back(getCoord());
//  Vector3 point = coord_ - gravityRef_->getCoord();
//  Vector3 velItr = velocity_ - gravityRef_->getVelocity();
//  vector<Vector3> points;
//
//  glColor3f(1,1,1);
//  glPushMatrix();
//  Vector3 drawP = (point-gravityRef_->getCoord()) * (0.3/gravityRef_->getRadius());
//  glTranslatef(drawP[0], drawP[1], drawP[2]);
//  glutSolidSphere(0.05, 10, 10);
//  glPopMatrix();
//  glColor3f(1,1,0);
//  double maxDist = 0.001;
//  double minDist = distance;
//  
//  double mult = 50.0;
//  double speedReduce = Gamani::getInstance().getSpeedReduce();
//
//  for (int i=0; i<300; ++i) {
//    Vector3 drawP = (point)/* * (0.3/gravityRef_->getRadius())*/;
//    double dist = sqrt(point[0]*point[0] + point[1]*point[1]);
//    mult = (dist - gravityRef_->getRadius() + 0.01) * 15 / speedReduce;
//    //mult *= speedReduce;
//    if (i%10 == 0) {
//      points.push_back(drawP);
//    }
//    if (dist < gravityRef_->getRadius()) {
//      break;
//    }
//    if (dist > maxDist) {
//      maxDist = dist;
//    }
//    if (dist < minDist) {
//      minDist = dist;
//    }
//    glVertex3f(drawP[0], drawP[1], drawP[2]);
//
//
//    Vector3 coordFrom = Vector3(0,0,0);
//    Vector3 coordTo = point;
//    double massFrom = gravityRef_->getMass() * 1e6; //kg
//    double massTo = getMass() * 1e6; //kg
//    //G*m*m/r^2
//    static double G = 6.6725e-11;
//    double distSquare = (coordFrom[0]-coordTo[0])*(coordFrom[0]-coordTo[0])+(coordFrom[1]-coordTo[1])*(coordFrom[1]-coordTo[1])+(coordFrom[2]-coordTo[2])*(coordFrom[2]-coordTo[2]);
//    distSquare *= 1e6*1e6; //meters^2
//    double force = G*massFrom*massTo*(1/distSquare); //kg^2/m^2*M*m^2/kg^2 = N
//    //if (!body2->isStatic()) {
//    Vector3 velocity = velItr * (mult *speedReduce);
//    //velocity *= mult; //m/s
//    Vector3 dir = coordFrom - coordTo;
//    dir.normalize();
//    //f = m*a; a=f/m; a=dv
//    double dVelocity = force/massTo*mult*mult *speedReduce * speedReduce;
//    velocity += Vector3(dir[0]*dVelocity, dir[1]*dVelocity, dir[2]*dVelocity);
//    
//    point *= 1.0e6;
//    point += velocity;
//    point *= 1.0/1.0e6;
//
//    velItr = velocity * (1/mult/speedReduce);
//  }
//
//  glScalef(1/maxDist*0.7, 1/maxDist*0.7, 1/maxDist*0.7);
//
//  glPushMatrix();
//  static double plAng = 0;
//  glRotatef(75, 1, 0, 0);
//  glRotatef(plAng, 0, 0, 1);
//  plAng += 0.05;
//  if (plAng > 360) {
//    plAng = 0;
//  }
//  glutWireSphere(gravityRef_->getRadius(), 15, 10);
//  glPopMatrix();
//
//  glBegin(GL_LINE_STRIP);
//  for (uint32_t i=0; i<points.size(); ++i) {
//    glVertex3f(points[i][0], -points[i][1], points[i][2]);
//  }
//  glEnd();
//
//  glPopMatrix();
//  glMatrixMode(GL_PROJECTION);
//  glPopMatrix();
//
//  glEnable(GL_LIGHTING);
//
//
//  Renderer::getInstance().textOut(-0.9, 0.6, 0, "A: %.1f", maxDist);
//  Renderer::getInstance().textOut(-0.9, 0.5, 0, "P: %.1f", minDist);
//  Renderer::getInstance().resetViewPort();
//}

void Ship::setDockedCoord()
{
  Vector3 refCoord = dockedTo_->getCoord();
  Vector3 refPort = dockedTo_->getDockingPort();
  Vector3 myPort = getDockingPort();
  double refPortLength = refPort.getLength()*dockedTo_->getRadius() + myPort.getLength()*getRadius();
  double refYaw = dockedTo_->getYaw();
  refYaw += 180;
  double myYaw = getYaw();
  Vector3 res = refCoord;
  res += Vector3(refPortLength*cos(refYaw*3.14159265/180.0), refPortLength*sin(refYaw*3.14159265/180.0), 0);
  velocity_ = dockedTo_->getVelocity();
  coord_ = res;
  double newYaw = refYaw + dockedTo_->getPortAngle();
  yaw_ = newYaw;
  while (yaw_ > 360.0) {
    yaw_ -= 360.0;
  }
}

void Ship::undock()
{
  docked_ = false;
  //dockedTo_ = NULL;
}

vector<Vector3> Ship::calcOrbit(AstralBody* from, AstralBody* to, double& minDist, double& maxDist)
{
  vector<Vector3> points;
  if (!from || !to) {
    return points;
  }
  Vector3 point = to->getCoord() - from->getCoord();
  Vector3 velItr = to->getVelocity() - from->getVelocity();


  double mult = 50.0;
  double speedReduce = Gamani::getInstance().getSpeedReduce();

  for (int i=0; i<3000; ++i) {
    Vector3 drawP = (point)/* * (0.3/gravityRef_->getRadius())*/;
    double dist = sqrt(point[0]*point[0] + point[1]*point[1]);
    mult = (dist - from->getRadius() + 0.01) * 2 / speedReduce;
    //mult = 20 / speedReduce;
    //mult *= speedReduce;
    if (i%10 == 0) {
      points.push_back(drawP);
    }
    if (dist < from->getRadius()) {
      break;
    }
    if (dist > maxDist) {
      maxDist = dist;
    }
    if (dist < minDist) {
      minDist = dist;
    }
    glVertex3f(drawP[0], drawP[1], drawP[2]);


    Vector3 coordFrom = Vector3(0,0,0);
    Vector3 coordTo = point;
    double massFrom = from->getMass() * 1e6; //kg
    double massTo = to->getMass() * 1e6; //kg
    //G*m*m/r^2
    static double G = 6.6725e-11;
    double distSquare = (coordFrom[0]-coordTo[0])*(coordFrom[0]-coordTo[0])+(coordFrom[1]-coordTo[1])*(coordFrom[1]-coordTo[1])+(coordFrom[2]-coordTo[2])*(coordFrom[2]-coordTo[2]);
    distSquare *= 1e6*1e6; //meters^2
    double force = G*massFrom*massTo*(1/distSquare); //kg^2/m^2*M*m^2/kg^2 = N
    //if (!body2->isStatic()) {
    Vector3 velocity = velItr * (mult *speedReduce);
    //velocity *= mult; //m/s
    Vector3 dir = coordFrom - coordTo;
    dir.normalize();
    //f = m*a; a=f/m; a=dv
    double dVelocity = force/massTo*mult*mult *speedReduce * speedReduce;
    velocity += Vector3(dir[0]*dVelocity, dir[1]*dVelocity, dir[2]*dVelocity);

    point *= 1.0e6;
    point += velocity;
    point *= 1.0/1.0e6;

    velItr = velocity * (1/mult/speedReduce);
  }
  return points;
}

//void Ship::testNavCom1()
//{
//  if (!gravityRef_) {
//    return;
//  }
//  updateGravityRef();
//  Renderer::getInstance().requestViewPort(Renderer::getInstance().getWidth() - 300 ,0, 300, 300);
//
//  Renderer::getInstance().textOut(-0.9, 0.8, 0, "%s", gravityRef_->getName().operator const char *());
//  double distance = sqrt((test1_->getCoord()[0]-gravityRef_->getCoord()[0])*(test1_->getCoord()[0]-gravityRef_->getCoord()[0]) + 
//    (test1_->getCoord()[1]-gravityRef_->getCoord()[1])*(test1_->getCoord()[1]-gravityRef_->getCoord()[1]));
//  Vector3 relSpd = velocity_ - gravityRef_->getVelocity();
//  Renderer::getInstance().textOut(-0.9, 0.7, 0, "Dist: %.1f RelSpd: (%.1f, %.1f)", distance, relSpd[0], relSpd[1]);
//
//  glDisable(GL_LIGHTING);
//
//  glMatrixMode(GL_PROJECTION);
//  glPushMatrix();
//  glLoadIdentity();
//  glMatrixMode(GL_MODELVIEW);
//  glPushMatrix();
//  glLoadIdentity();
//
//  glBegin(GL_LINE_LOOP);
//  glVertex3f(-0.9, -0.9, 0);
//  glVertex3f(0.9, -0.9, 0);
//  glVertex3f(0.9, 0.9, 0);
//  glVertex3f(-0.9, 0.9, 0);
//  glEnd();
//
//  glBegin(GL_LINE_LOOP);
//  glVertex3f(0.7, 0.9, 0);
//  glVertex3f(0.9, 0.9, 0);
//  glVertex3f(0.9, 0.7, 0);
//  glVertex3f(0.7, 0.7, 0);
//  glEnd();
//
//
//  Vector3 direction = test1_->getCoord() - gravityRef_->getCoord();
//  direction.normalize();
//  direction *= 0.1;
//  glBegin(GL_LINES);
//  glVertex3f(0.8, 0.8, 0);
//  glVertex3f(0.8 - direction[0], 0.8 + direction[1], 0);
//  glEnd();
//
//  glColor3f(0,1,0);
//  double maxDist = 0.001;
//  double minDist = distance;
//
//  vector<Vector3> points = calcOrbit(test1_, test2_, minDist, maxDist);
//  vector<Vector3> points1 = calcOrbit(test1_, this, minDist, maxDist);
//
//  glScalef(1/maxDist*0.7, 1/maxDist*0.7, 1/maxDist*0.7);
//
//  glPushMatrix();
//  static double plAng = 0;
//  glRotatef(75, 1, 0, 0);
//  glRotatef(plAng, 0, 0, 1);
//  plAng += 0.05;
//  if (plAng > 360) {
//    plAng = 0;
//  }
//  glutWireSphere(gravityRef_->getRadius(), 15, 10);
//  glPopMatrix();
//
//  glBegin(GL_LINE_STRIP);
//  for (uint32_t i=0; i<points.size(); ++i) {
//    glVertex3f(points[i][0], -points[i][1], points[i][2]);
//  }
//  glEnd();
//
//  glBegin(GL_LINE_STRIP);
//  for (uint32_t i=0; i<points1.size(); ++i) {
//    glVertex3f(points1[i][0], -points1[i][1], points1[i][2]);
//  }
//  glEnd();
//
//  glPopMatrix();
//  glMatrixMode(GL_PROJECTION);
//  glPopMatrix();
//
//  glEnable(GL_LIGHTING);
//
//
//  Renderer::getInstance().textOut(-0.9, 0.6, 0, "A: %.1f", maxDist);
//  Renderer::getInstance().textOut(-0.9, 0.5, 0, "P: %.1f", minDist);
//  Renderer::getInstance().resetViewPort();
//}


void Ship::yawLeft()
{
  yawLeft(yawPower_);
}

void Ship::yawLeft(double val)
{
  if (docked_) {
    return;
  }
  if (val > yawPower_) {
    val = yawPower_;
  }
  yawVel_ -= val;
  if (yawVel_ < -maxYawVel_) {
    yawVel_ = -maxYawVel_;
  }
}

void Ship::yawRight()
{
  yawRight(yawPower_);
}

void Ship::yawRight(double val)
{
  if (docked_) {
    return;
  }
  if (val > yawPower_) {
    val = yawPower_;
  }
  yawVel_ += val;
  if (yawVel_ >= maxYawVel_) {
    yawVel_ = maxYawVel_;
  }
}

void Ship::accelerate()
{
  if (docked_) {
    return;
  }
  landed_ = false;
  double yawAngleRad = yaw_ * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] + dx*marchPower_, velocity_[1] + dy*marchPower_, velocity_[2]);
}

void Ship::back()
{
  if (docked_) {
    return;
  }
  landed_ = false;
  double yawAngleRad = yaw_ * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] - dx*marchPower_*0.1, velocity_[1] - dy*marchPower_*0.1, velocity_[2]);
}

void Ship::steerLeft()
{
  if (docked_) {
    return;
  }
  landed_ = false;
  double yawAngleRad = (yaw_ + 90.0) * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] - dx*marchPower_*0.1, velocity_[1] - dy*marchPower_*0.1, velocity_[2]);
}

void Ship::steerRight()
{
  if (docked_) {
    return;
  }
  landed_ = false;
  double yawAngleRad = (yaw_ - 90.0) * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] - dx*marchPower_*0.1, velocity_[1] - dy*marchPower_*0.1, velocity_[2]);
}

void Ship::setAutopilotTo(Autopilot::ProgID prog)
{
  autopilot_->clearQueue();
  switch (prog) {
  case Autopilot::KillRot:
    autopilot_->addProgram(new KillRotProg(autopilot_));
    break;
  case Autopilot::Launch:
    autopilot_->addProgram(new LaunchProg(autopilot_));
    break;
  case Autopilot::Rotate:
    autopilot_->addProgram(new RotateProg(autopilot_, yaw_ + 180.0));
    break;
  case Autopilot::Approach:
    autopilot_->addProgram(new ApproachProg(autopilot_));
    break;
  }
  //currProg_ = prog;
  //if (prog == Launch) {
  //  if (!landed_) {
  //    return;
  //  }
  //  launchCount_ = 0;
  //  landed_ = false;
  //  landedOn_ = NULL;
  //} else if (prog == Rotate) {
  //  targetAngle_ = yaw_ + 90;
  //  if (targetAngle_ < 0) {
  //    targetAngle_ += 360;
  //  }
  //  startAngle_ = yaw_;
  //  rotateRight_ = true;
  //  rotatePhase1_ = true;
  //} else if (prog == Approach) {
  //  startAngle_ = yaw_;
  //  targetCoords_ = gravityRef_->getCoord();
  //  Vector3 dir = gravityRef_->getCoord() - coord_;
  //  double dist = dir.getLength();
  //  targetDist_ = dist*0.99/2.0;
  //  targetAngle_ = acos(dir[0]/dir[1]) * 180.0 / 3.14159265 - 90;
  //  if (fabs(dir[1]) < 0.001) {
  //    targetAngle_ = (dir[1] > 0)? 270 : 90;
  //  }


  //  {
  //    double deltaYaw = targetAngle_ - yaw_;
  //    while (deltaYaw < 0) {
  //      deltaYaw += 360;
  //    }
  //    targetAngle_ =  yaw_ + deltaYaw / 2.0;
  //    while (targetAngle_ > 360) {
  //      targetAngle_ -= 360;
  //    }
  //    if (deltaYaw > 180) {
  //      rotateRight_ = false;
  //    } else {
  //      rotateRight_ = true;
  //    }
  //  }

  //  currProg_ = Rotate;
  //  rotatePhase1_ = true;
  //  programs_.push_front(Accelerate);
  //  programs_.push_front(Rotate);
  //  programs_.push_front(Stop);
  //}
}

void Ship::autopilotStep()
{
  autopilot_->step();
  //if (currProg_ == KillRot) {
  //  double val = yawVel_;
  //  if (abs(yawVel_) < yawPower_/10.0f) {
  //    yawVel_ = 0;
  //    currProg_ = NoProgram;
  //  } else if (yawVel_ > 0) {
  //    yawLeft();
  //  } else {
  //    yawRight();
  //  }
  //} else if (currProg_ == Rotate) {
  //  if (targetAngle_ < 0) {
  //    targetAngle_ = yaw_ + 90;
  //    if (targetAngle_ < 0) {
  //      targetAngle_ += 360;
  //    }
  //    startAngle_ = yaw_;
  //    rotateRight_ = true;
  //    rotatePhase1_ = true;
  //  }
  //  if (rotatePhase1_) {
  //    if (rotateRight_) {
  //      yawRight();
  //    } else {
  //      yawLeft();
  //    }
  //    if (abs(yaw_ - targetAngle_) < yawVel_*1.1) {
  //      rotatePhase1_ = false;
  //      cout << "Phase 2 " << yaw_ << endl;
  //    }
  //  } else {
  //    if (rotateRight_) {
  //      yawLeft();
  //    } else {
  //      yawRight();
  //    }
  //    if (yawVel_ < yawPower_*1.5) {
  //      yawVel_ = 0;
  //      rotatePhase1_ = true;
  //      currProg_ = NoProgram;
  //      targetAngle_ = -100;
  //    }
  //  }
  //} else if (currProg_ == Launch) {
  //  //++launchCount_;
  //  //if (launchCount_ > 10000) {
  //  //  currProg_ = NoProgram;
  //  //}
  //  //accelerate();
  //  double distance = sqrt((coord_[0] - gravityRef_->getCoord()[0])*(coord_[0] - gravityRef_->getCoord()[0]) + 
  //    (coord_[1] - gravityRef_->getCoord()[1])*(coord_[1] - gravityRef_->getCoord()[1]));
  //  if (distance > (gravityRef_->getRadius() + radius_) * 1.01) {
  //    currProg_ = NoProgram;
  //  }
  //} else if (currProg_ == Accelerate) {
  //  Vector3 dir = targetCoords_ - coord_;
  //  double dist = dir.getLength();
  //  if (dist < targetDist_) {
  //    currProg_ = NoProgram;
  //  }
  //  accelerate();
  //} else if (currProg_ == Stop) {
  //  Vector3 dir = targetCoords_ - coord_;
  //  double dist = dir.getLength();
  //  if (dist > targetDist_) {
  //    currProg_ = NoProgram;
  //  }
  //  accelerate();
  //} else if (currProg_ == NoProgram && programs_.size() > 0) {
  //  currProg_ = programs_.back();
  //  cout << "Switching to: " << getProgName(currProg_) << endl;
  //  programs_.pop_back();
  //}
}

void Ship::attemptDocking(Station* station)
{
  Vector3 stationPort = station->getCoord() + station->getDockingPort();
  Vector3 myPort = getCoord() + getDockingPort();
  double portDist = (stationPort - myPort).getLength();
  if (dockedTo_ && !docked_) {
    //Currently undocking
    return;
  }
  if (portDist < 1.5) {
    double stationPortAngle = station->getPortAngle() + station->getYaw();
    double myPortAngle = getPortAngle() + getYaw();
    double angleDelta = stationPortAngle - myPortAngle - 180.0;
    while (angleDelta < 0) {
      angleDelta += 360.0;
    }
    while (angleDelta >= 360.0) {
      angleDelta -= 360.0;
    }
    if (angleDelta < 5 || angleDelta > 355) {
      docked_ = true;
      station->dock(this);
      dockedTo_ = station;
      return;
    }
  }
  Gamani::getInstance().pause();
  //Crash!!!
}

void Ship::checkUndocking()
{
  if (dockedTo_ && !docked_) {
    //Currently undocking
    double dist = (dockedTo_->getCoord() - getCoord()).getLength();
    //double ddd = (dockedTo_->getRadius() + getRadius())*1.2;
    if (dist > (dockedTo_->getRadius() + getRadius())*2) {
      //Undocking completed
      dockedTo_->undock(this);
      dockedTo_ = NULL;
    }
  }
}

void Ship::collideWith(AstralBody* body)
{
  //if (currProg_ == Launch) {
  //  return;
  //}
  if (isDocked()) {
    return;
  }
  if (body->getType() == Renderable::StationType) {
    attemptDocking((Station*)body);
    return;
  }
  landed_ = true;
  //velocity_ = Vector3(0,0,0);
  yawVel_ = 0;
  //currProg_ = NoProgram;
  Vector3 dir = coord_ - body->getCoord();
  dir[1] = -dir[1];
  double angle = asin(dir[0]/sqrt(dir[0]*dir[0] + dir[1]*dir[1]));
  yaw_ = angle/3.14159265*180.0f;
  if (dir[0] < 0 && dir[1] < 0) {
    yaw_ = 180 - yaw_;
  } else if (dir[1] < 0) {
    yaw_ = 180 - yaw_;
  }
  while(yaw_ < 0) {
    yaw_ += 360;
  }
  while (yaw_ >= 360) {
    yaw_ -= 360;
  }
  landedOn_ = body;
  //gravityRef_ = body;
}

Vector3 Ship::getLandedCoords()
{
  assert(landedOn_ && landed_);
  Vector3 res = landedOn_->getCoord();
  double dx = landedOn_->getRadius() * sin(yaw_*3.14159265/180.0);
  double dy = -landedOn_->getRadius() * cos(yaw_*3.14159265/180.0);
  res[0] += dx;
  res[1] += dy;
  return res;
}

void Ship::setHUD(HUD* hud)
{
  delete hud_;
  hud_ = hud;
  hud->assignTo(this);
}

CString Ship::getCurrProgName()
{
  Autopilot::ProgID id = autopilot_->getCurrProg();
  switch (id) {
  case Autopilot::NoProgram:
    return "No Program";
  case Autopilot::KillRot:
    return "Kill rotation";
  case Autopilot::Rotate:
    return "Rotation";
  case Autopilot::Launch:
    return "Launch";
  case Autopilot::Approach:
    return "Approach";
  case Autopilot::Accelerate:
    return "Accelerate";
  case Autopilot::Stop:
    return "Stop";
  default:
    return "UnKnown";
  }
}

bool Ship::isLaunching()
{
  return autopilot_->getCurrProg() == Autopilot::Launch;
}
