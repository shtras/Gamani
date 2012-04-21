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
  marchPower_ = 0.5;
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
  initializeAsPlayerControlled_ = false;
  autopilot_ = new Autopilot(this);
  for (int i=0; i<6; ++i) {
    engineStates_[i] = 0;
  }
  inAtmosphereOf_ = NULL;
}

Ship::~Ship()
{
  delete hud_;
  delete autopilot_;
}

void Ship::engageEngine(Engine engine, float power/* = 1.0*/)
{
  engineStates_[engine] = power;
}

void Ship::updateEngines()
{
  for (int i=0; i<6; ++i) {
    engineStates_[i] -= 0.1f;
    if (engineStates_[i] < 0) {
      engineStates_[i] = 0;
    }
  }
}

void Ship::drawEngines()
{
  glDisable(GL_LIGHTING);
  double radius = getRadius() * GLOBAL_MULT;
  glColor3f(0.5, 0.9, 0.3);

  if (engineStates_[March] > 0) {
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    glTranslatef(0, 0, radius*0.6);
    glutSolidCone(radius * 0.1 * engineStates_[March], radius * engineStates_[March], 10, 10);
    glPopMatrix();
  }

  if (engineStates_[Reverse] > 0) {
    glPushMatrix();
    glTranslatef(0, 0, radius*0.6);
    glutSolidCone(radius * 0.05 * engineStates_[Reverse], radius * engineStates_[Reverse] * 0.5, 10, 10);
    glPopMatrix();
  }

  if (engineStates_[FrontLeft] > 0) {
    glPushMatrix();
    glTranslatef(radius*0.2, 0, radius*0.5);
    glRotatef(90, 0, 1, 0);
    glutSolidCone(radius * 0.02 * engineStates_[FrontLeft], radius * engineStates_[FrontLeft] * 0.2, 10, 10);
    glPopMatrix();
  }

  if (engineStates_[RearLeft] > 0) {
    glPushMatrix();
    glTranslatef(radius*0.5, 0, -radius*0.5);
    glRotatef(90, 0, 1, 0);
    glutSolidCone(radius * 0.02 * engineStates_[RearLeft], radius * engineStates_[RearLeft] * 0.2, 10, 10);
    glPopMatrix();
  }

  if (engineStates_[FrontRight] > 0) {
    glPushMatrix();
    glTranslatef(-radius*0.2, 0, radius*0.5);
    glRotatef(-90, 0, 1, 0);
    glutSolidCone(radius * 0.02 * engineStates_[FrontRight], radius * engineStates_[FrontRight] * 0.2, 10, 10);
    glPopMatrix();
  }

  if (engineStates_[RearRight] > 0) {
    glPushMatrix();
    glTranslatef(-radius*0.5, 0, -radius*0.5);
    glRotatef(-90, 0, 1, 0);
    glutSolidCone(radius * 0.02 * engineStates_[RearRight], radius * engineStates_[RearRight] * 0.2, 10, 10);
    glPopMatrix();
  }

  glEnable(GL_LIGHTING);
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
  glTranslatef(coord[0]*GLOBAL_MULT, coord[1]*GLOBAL_MULT, coord[2]*GLOBAL_MULT);

  glRotatef(-90, 1, 0, 0);
  glRotatef(180-yaw_, 0, 1, 0);
  //glTranslatef(0, 0, -radius_*GLOBAL_MULT/2.0);

  const Model* model = getModel();
  if (model) {
    glPushMatrix();
    glScalef(radius_*GLOBAL_MULT, radius_*GLOBAL_MULT, radius_*GLOBAL_MULT);
    if (Gamani::getInstance().drawBoundBoxes()) {
      glDisable(GL_LIGHTING);
      glutWireSphere(1, 10, 10);
      glEnable(GL_LIGHTING);
    }

    glEnable(GL_LIGHT2);
    model->draw();
    glDisable(GL_LIGHT2);

    //renderPort();

    glPopMatrix();
  } else {
    glutSolidCone(radius_*GLOBAL_MULT/4.0f, radius_*GLOBAL_MULT, 10, 5);
  }


  drawName();

  drawEngines();

  glPopMatrix();
}

void Ship::setDockedCoord()
{
  //Vector3 refCoord = dockedTo_->getCoord();

  //double refPortAngle = dockedTo_->getPortAngle() + dockedTo_->getYaw();
  //refPortAngle = DegToRad(refPortAngle);
  //Vector3 refPort = refCoord + Vector3(sin(refPortAngle), cos(refPortAngle), 0) * dockedTo_->getPortDist()*dockedTo_->getRadius();

  //double myPortAngle = getPortAngle() + getYaw();
  //myPortAngle = DegToRad(myPortAngle);
  //Vector3 myPort = getCoord() + Vector3(sin(myPortAngle), cos(myPortAngle), 0) * getPortDist() * getRadius();

  //double refPortLength = refPort.getLength()*dockedTo_->getRadius() + myPort.getLength()*getRadius();
  //double refYaw = dockedTo_->getYaw();
  //refYaw += 180;
  //double myYaw = getYaw();
  //Vector3 res = refCoord;
  //res += Vector3(refPortLength*cos(refYaw*3.14159265/180.0), refPortLength*sin(refYaw*3.14159265/180.0), 0);
  //velocity_ = dockedTo_->getVelocity();
  //coord_ = res;
  //double newYaw = refYaw + dockedTo_->getPortAngle();
  //yaw_ = newYaw;
  //while (yaw_ > 360.0) {
  //  yaw_ -= 360.0;
  //}
  double refPortAngle = dockedTo_->getYaw() + dockedTo_->getPortAngle();
  refPortAngle = DegToRad(refPortAngle);
  Vector3 refPort = dockedTo_->getCoord() + Vector3(sin(refPortAngle), cos(refPortAngle), 0)*dockedTo_->getPortDist()*dockedTo_->getRadius();
  
  double myYaw = dockedTo_->getYaw() + dockedTo_->getPortAngle() - 180.0 - getPortAngle();
  setYaw(myYaw);
  myYaw = DegToRad(myYaw);

  Vector3 myCoord = refPort - Vector3(sin(myYaw), cos(myYaw), 0)*getPortDist()*getRadius();
  setCoord(myCoord);
  setVelocity(dockedTo_->getVelocity());

  double dist = (myCoord - dockedTo_->getCoord()).getLength();
  double radSum = getRadius() + dockedTo_->getRadius();
  int bbb = 0;

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

void Ship::yawLeft()
{
  yawLeft(1);
}

void Ship::yawLeft(double val)
{
  if (docked_) {
    return;
  }
  //if (val > yawPower_) {
  //  val = yawPower_;
  //}
  if (val > 1) {
    val = 1;
  }
  if (val < 0) {
    val = 0;
  }
  engageEngine(RearLeft, val);
  engageEngine(FrontRight, val);
  val *= yawPower_;
  yawVel_ -= val;
  if (yawVel_ < -maxYawVel_) {
    yawVel_ = -maxYawVel_;
  }
}

void Ship::yawRight()
{
  yawRight(1);
}

void Ship::yawRight(double val)
{
  if (docked_) {
    return;
  }
  //if (val > yawPower_) {
  //  val = yawPower_;
  //}
  if (val > 1) {
    val = 1;
  }
  if (val < 0) {
    val = 0;
  }
  engageEngine(RearRight, val);
  engageEngine(FrontLeft, val);
  val *= yawPower_;
  yawVel_ += val;
  if (yawVel_ >= maxYawVel_) {
    yawVel_ = maxYawVel_;
  }
}

void Ship::accelerate(double fraction/* = 1.0*/)
{
  if (docked_) {
    return;
  }
  engageEngine(March, fraction);
  landed_ = false;
  double yawAngleRad = yaw_ * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] + dx*marchPower_ * fraction, velocity_[1] + dy*marchPower_ * fraction, velocity_[2]);
}

void Ship::back(double fraction/* = 1.0*/)
{
  if (docked_) {
    return;
  }
  engageEngine(Reverse, fraction);
  landed_ = false;
  double yawAngleRad = yaw_ * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] - dx*marchPower_*0.5 * fraction, velocity_[1] - dy*marchPower_*0.5 * fraction, velocity_[2]);
}

void Ship::steerLeft(double fraction/* = 1.0*/)
{
  if (docked_) {
    return;
  }
  engageEngine(RearRight, fraction);
  engageEngine(FrontRight, fraction);
  landed_ = false;
  double yawAngleRad = (yaw_ + 90.0) * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] - dx*marchPower_*0.1 * fraction, velocity_[1] - dy*marchPower_*0.1 * fraction, velocity_[2]);
}

void Ship::steerRight(double fraction/* = 1.0*/)
{
  if (docked_) {
    return;
  }
  engageEngine(RearLeft, fraction);
  engageEngine(FrontLeft, fraction);
  landed_ = false;
  double yawAngleRad = (yaw_ - 90.0) * 3.14159265f / 180.0f;
  double dx = sin(yawAngleRad);
  double dy = -cos(yawAngleRad);
  velocity_ = Vector3(velocity_[0] - dx*marchPower_*0.1 * fraction, velocity_[1] - dy*marchPower_*0.1 * fraction, velocity_[2]);
}

void Ship::setAutopilotTo(Autopilot::ProgID prog)
{
  //autopilot_->clearQueue();
  switch (prog) {
  case Autopilot::KillRot:
    //autopilot_->enqueue(new KillRotProg(autopilot_));
    autopilot_->loadProg("res/asm/killrot.asm");
    break;
  case Autopilot::Launch:
    autopilot_->enqueue(new LaunchProg(autopilot_));
    break;
  case Autopilot::Rotate:
    autopilot_->enqueue(new RotateProg(autopilot_, yaw_ + 180.0));
    break;
  case Autopilot::Approach:
    autopilot_->enqueue(new ApproachProg(autopilot_));
    break;
  case Autopilot::ProGrade:
    autopilot_->enqueue(new ProGradeProg(autopilot_));
    break;
  case Autopilot::RetroGrade:
    autopilot_->enqueue(new RetroGradeProg(autopilot_));
    break;
  case Autopilot::EqSpeed:
    autopilot_->enqueue(new EqSpeedProg(autopilot_));
    break;
  case Autopilot::Orbit:
    autopilot_->enqueue(new OrbitProg(autopilot_));
    break;
  case Autopilot::RotateTo:
    autopilot_->enqueue(new RotateToProg(autopilot_));
    break;
  case Autopilot::RotateFrom:
    autopilot_->enqueue(new RotateFromProg(autopilot_));
    break;
  case Autopilot::PerpendOrbit:
    autopilot_->enqueue(new PrependicularizeSpeedProg(autopilot_));
    break;
  default:
    assert(0);
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
  double stationPortAngle = station->getYaw() + station->getPortAngle();
  stationPortAngle = DegToRad(stationPortAngle);
  Vector3 stationPort = station->getCoord() + Vector3(sin(stationPortAngle), cos(stationPortAngle), 0) * station->getPortDist() * station->getRadius();

  double myPortAngle = getYaw() + getPortAngle();
  myPortAngle = DegToRad(myPortAngle);
  Vector3 myPort = getCoord() + Vector3(sin(myPortAngle), cos(myPortAngle), 0) * getPortDist() * getRadius();

  double portDist = (stationPort - myPort).getLength();
  if (dockedTo_ && !docked_) {
    //Currently undocking
    return;
  }
  if (portDist < 0.1) {
    double stationPortAngle = station->getPortAngle() + station->getYaw();
    double myPortAngle = getPortAngle() + getYaw();
    double angleDelta = stationPortAngle - myPortAngle - 180.0;
    while (angleDelta < 0) {
      angleDelta += 360.0;
    }
    while (angleDelta >= 360.0) {
      angleDelta -= 360.0;
    }
    if (angleDelta < 15 || angleDelta > 345) {
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
    if (dist > (dockedTo_->getRadius() + getRadius())*1.1) {
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
    attemptDocking(static_cast<Station*>(body));
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
  double dx = (landedOn_->getRadius() + radius_) * sin(yaw_*3.14159265/180.0);
  double dy = -(landedOn_->getRadius() + radius_) * cos(yaw_*3.14159265/180.0);
  res[0] += dx;
  res[1] += dy;
  return res;
}

Vector3 Ship::getLandedVel()
{
  assert (landedOn_ && landed_);
  Vector3 res = landedOn_->getVelocity();
  double surfaceVel = landedOn_->getSurfaceLinearSpeed();
  double dx = surfaceVel * -cos(DegToRad(yaw_));
  double dy = surfaceVel * -sin(DegToRad(yaw_));
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

bool Ship::isLaunching()
{
  return autopilot_->getCurrProg() == Autopilot::Launch;
}
