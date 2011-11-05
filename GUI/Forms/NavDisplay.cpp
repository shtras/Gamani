#include "StdAfx.h"
#include "NavDisplay.h"
#include "Ship.h"
#include "Station.h"
#include "Gamani.h"
#include "Renderer.h"

NavDisplay::NavDisplay():syncOrbitRef_(NULL), mode_(Orbit),selectRef_(false),gravityRef_(NULL),manualRef_(false),dp_(0),da_(0)
{
}

NavDisplay::~NavDisplay()
{
}

void NavDisplay::init()
{
  setDimensions(0, 0.4, 0.4, 0.4);
  square_ = true;
  shipYawText_ = new WText();
  shipYawText_->setDimensions(0.05, 0.95, 1, 1);
  shipYawText_->setText("Hello!");
  addWidget(shipYawText_);

  modeName_ = new WText();
  modeName_->setDimensions(0.5, 0.95, 1, 1);
  modeName_->setText("Orbit mode");
  addWidget(modeName_);

  gravityRefName_ = new WText();
  gravityRefName_->setDimensions(0.05, 0.9, 1, 1);
  gravityRefName_->setText("Hello!");
  addWidget(gravityRefName_);

  velData_ = new WText();
  velData_->setDimensions(0.05, 0.85, 1, 1);
  velData_->setText("AAA!");
  addWidget(velData_);

  distData_ = new WText();
  distData_->setDimensions(0.05, 0.80, 1, 1);
  distData_->setText("AAA!");
  addWidget(distData_);

  surfDistData_ = new WText();
  surfDistData_->setDimensions(0.05, 0.75, 1, 1);
  surfDistData_->setText("AAA!");
  addWidget(surfDistData_);

  apoText_ = new WText();
  apoText_->setDimensions(0.05, 0.7, 1, 1);
  addWidget(apoText_);

  periText_ = new WText();
  periText_->setDimensions(0.05, 0.65, 1, 1);
  addWidget(periText_);

  modeButton_ = new WButton();
  modeButton_->setDimensions(0.41, 0.01, 0.19, 0.07);
  modeButton_->sigClick.connect(this, &NavDisplay::modeButtonClick);
  modeButton_->setLabel("Mode");
  addWidget(modeButton_);

  refButton_ = new WButton();
  refButton_->setDimensions(0.01, 0.01, 0.19, 0.07);
  refButton_->sigClick.connect(this, &NavDisplay::refButtonClick);
  refButton_->setLabel("Ref");
  addWidget(refButton_);

  autoRefButton_ = new WButton();
  autoRefButton_->setDimensions(0.21, 0.01, 0.19, 0.07);
  autoRefButton_->sigClick.connect(this, &NavDisplay::autoRefButtonClick);
  autoRefButton_->setLabel("Auto");
  addWidget(autoRefButton_);

  tgtToSyncOrbitButton_ = new WButton();
  tgtToSyncOrbitButton_->setDimensions(0.61, 0.01, 0.19, 0.07);
  tgtToSyncOrbitButton_->sigClick.connect(this, &NavDisplay::tgtToSyncOrbitButtonClick);
  tgtToSyncOrbitButton_->setLabel("Target");
  tgtToSyncOrbitButton_->setVisible(false);
  addWidget(tgtToSyncOrbitButton_);

  modeMenu_ = new WMenu();
  modeMenu_->setDimensions(0.04, 0.54, 0.22, 0.4);
  /////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode1Item = new WMenu::Item();
  mode1Item->setDimensions(0.05,0.85,0.4,0.08);
  mode1Item->setLabel("Orbit");
  modeMenu_->addItem(mode1Item);
  mode1Item->setValue((void*)1);
  mode1Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode1Item);
  //////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode2Item = new WMenu::Item();
  mode2Item->setDimensions(0.05,0.75,0.4,0.08);
  mode2Item->setLabel("Sync orbit");
  modeMenu_->addItem(mode2Item);
  mode2Item->setValue((void*)2);
  mode2Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode2Item);
  //////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode3Item = new WMenu::Item();
  mode3Item->setDimensions(0.05,0.65,0.4,0.08);
  mode3Item->setLabel("Docking");
  modeMenu_->addItem(mode3Item);
  mode3Item->setValue((void*)3);
  mode3Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode3Item);
  //////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode4Item = new WMenu::Item();
  mode4Item->setDimensions(0.05,0.55,0.4,0.08);
  mode4Item->setLabel("Axes");
  modeMenu_->addItem(mode4Item);
  mode4Item->setValue((void*)4);
  mode4Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode4Item);
  //////////////////////////////////////////////////////////////////////////
  modeMenu_->setVisible(false);
  addWidget(modeMenu_);

  tgtSelectMenu_ = new WMenu();
  tgtSelectMenu_->setDimensions(0.04, 0.1, 0.32, 0.9);
  tgtSelectMenu_->setVisible(false);
  addWidget(tgtSelectMenu_);

  visible_ = true;
}

void NavDisplay::setTarget(void* tgt)
{
  tgtSelectMenu_->setVisible(false);
  AstralBody* newTgt = (AstralBody*)tgt;
  if (selectRef_) {
    setManualRef(true);
    setGravityRef(newTgt);
  } else {
    syncOrbitRef_ = newTgt;
  }
  tgtSelectMenu_->initialClear(this);
  addWidgetToGC(tgtSelectMenu_);
}

void NavDisplay::selectTargetStartingFrom(void* bodyRef)
{
  bool displayFree = false;
  bool freeList = false;
  AstralBody* body = NULL;
  if (bodyRef == (void*)-1) {
    freeList = true;
  } else if (!bodyRef) {
    body = *Gamani::getInstance().getWorld()->getCurrentSystem()->getStars().begin();
    displayFree = true;
  } else {
    body = (AstralBody*)bodyRef;
  }
  tgtSelectMenu_->initialClear(this);


  if (!freeList) {
    WMenu::Item* mainItem = new WMenu::Item();
    mainItem->setDimensions(0.05,0.9,0.4,0.07);
    mainItem->setValue((void*)body);
    mainItem->setLabel(body->getName());
    mainItem->sigClick.connect(this, &NavDisplay::setTarget);
    tgtSelectMenu_->addItem(mainItem);
    addWidget(mainItem);
  }
  
  vector<AstralBody*>* satellites = NULL;
  if (freeList) {
    satellites = &Gamani::getInstance().getWorld()->getFreeObjects();
  } else {
    satellites = &body->getSatellites();
  }
  unsigned int i = 0;
  for (; i<satellites->size(); ++i) {
    AstralBody* satellite = (*satellites)[i];

    WMenu::Item* item = new WMenu::Item();
    item->setDimensions(0.05, 0.83 - i*0.08, 0.4, 0.07);
    item->setValue((void*)satellite);
    item->setLabel(satellite->getName());
    if (freeList) {
      item->sigClick.connect(this, &NavDisplay::setTarget);
    } else {
      item->sigClick.connect(this, &NavDisplay::selectTargetStartingFrom);
    }
    tgtSelectMenu_->addItem(item);
    addWidget(item);
  }

  if (displayFree) {
    WMenu::Item* freeItem = new WMenu::Item();
    freeItem->setDimensions(0.05, 0.83 - (i)*0.08, 0.4, 0.07);
    freeItem->setValue((void*)-1);
    freeItem->setLabel("Ships");
    freeItem->sigClick.connect(this, &NavDisplay::selectTargetStartingFrom);
    tgtSelectMenu_->addItem(freeItem);
    addWidget(freeItem);
  }

  tgtSelectMenu_->setVisible(true);
}

void NavDisplay::tgtToSyncOrbitButtonClick()
{
  selectRef_ = false;
  selectTargetStartingFrom(NULL);
}

void NavDisplay::modeButtonClick()
{
  modeMenu_->setVisible(true);
  shipYawText_->setText("World!");
}

void NavDisplay::refButtonClick()
{
  selectRef_ = true;
  selectTargetStartingFrom(NULL);
}

void NavDisplay::autoRefButtonClick()
{
  setManualRef(false);
}

void NavDisplay::menuModeClick(void* val)
{
  int value = (int)val;
  switch (value) {
  case 1:
    modeName_->setText("Orbit mode");
    tgtToSyncOrbitButton_->setVisible(false);
    modeMenu_->setVisible(false);
    mode_ = Orbit;
    break;
  case 2:
    modeName_->setText("Sync orbit mode");
    tgtToSyncOrbitButton_->setVisible(true);
    modeMenu_->setVisible(false);
    mode_ = SyncOrbit;
    break;
  case 3:
    modeName_->setText("Docking mode");
    tgtToSyncOrbitButton_->setVisible(false);
    modeMenu_->setVisible(false);
    mode_ = Docking;
    break;
  case 4:
    modeName_->setText("Axes mode");
    tgtToSyncOrbitButton_->setVisible(false);
    modeMenu_->setVisible(false);
    mode_ = Axes;
    break;
  }
}

void NavDisplay::updateData()
{
  if (!manualRef_ || !gravityRef_) {
    updateGravityRef();
  }
  shipYawText_->setText(CString("Yaw: ") + CString(ship_->getYaw(), 2));
  gravityRefName_->setText(gravityRef_->getName());
  Vector3 relSpd = ship_->getVelocity() - gravityRef_->getVelocity();
  Vector3 relPos = ship_->getCoord() - gravityRef_->getCoord();
  double dist = relPos.getLength();
  double sdist = dist - gravityRef_->getRadius();

  

  CString vel = Renderer::getInstance().formatVelocity(relSpd.getLength(), 2);
  char sprStr[100];
  sprintf(sprStr, "V: %s", vel.operator const char *());
  velData_->setText(sprStr);

  sprintf(sprStr, "D: %s", Renderer::getInstance().formatDistance(dist, 2).operator const char *());
  distData_->setText(sprStr);

  sprintf(sprStr, "H: %s", Renderer::getInstance().formatDistance(sdist, 2));
  surfDistData_->setText(sprStr);

  sprintf(sprStr, "Da: %s", Renderer::getInstance().formatDistance(da_, 2));
  apoText_->setText(sprStr);

  sprintf(sprStr, "Dp: %s", Renderer::getInstance().formatDistance(dp_, 2));
  periText_->setText(sprStr);
}

void NavDisplay::drawSyncOrbit(Ship* ship, DynamicBody* ref)
{
  Vector3 r = (ship->getCoord() - gravityRef_->getCoord()) * 1e6;
  Vector3 v = (ship->getVelocity() - gravityRef_->getVelocity());
  double M = gravityRef_->getMass() * 1e6;
  Vector3 h = r*v;
  Vector3 n = Vector3(0,1,0)*h;
  double G = 6.6725e-11;
  double myu = G*M;
  Vector3 e = (r*(v.dot(v) - myu/r.getLength()) - v*(r.dot(v)))*(1.0/myu);
  double E = v.dot(v)/2.0 - myu/r.getLength();
  double a = -myu/2.0/E;
  double elen = e.getLength();
  double b = sqrt(a*a*(1.0-e.dot(e)));
  double theta = h[2]/h.getLength();
  double tau = acos(n.dot(e) / n.getLength() / e.getLength()) * 180.0 / 3.14159265;
  if (e[1] < 0) {
    tau = 360-tau;
  }
  double ab = a/b;

  double eps = 1.0-(b*b/(a*a));
  if (eps > 0) {
    eps = sqrt(eps);
  } else {
    eps = 0;
  }

  double maxDiameter = 2*a*1e-6;
  double translateDist = eps*a*1e-6;
  double scaleFactor = a/b * maxDiameter / 2.0;

  Vector3 r1 = (ref->getCoord() - gravityRef_->getCoord()) * 1e6;
  Vector3 v1 = (ref->getVelocity() - gravityRef_->getVelocity());
  double M1 = gravityRef_->getMass() * 1e6;
  Vector3 h1 = r1*v1;
  Vector3 n1 = Vector3(0,1,0)*h1;
  double G1 = 6.6725e-11;
  double myu1 = G1*M1;
  Vector3 e1 = (r1*(v1.dot(v1) - myu1/r1.getLength()) - v1*(r1.dot(v1)))*(1.0/myu1);
  double E1 = v1.dot(v1)/2.0 - myu1/r1.getLength();
  double a1 = -myu1/2.0/E1;
  double elen1 = e1.getLength();
  double b1 = sqrt(a1*a1*(1.0-e1.dot(e1)));
  double theta1 = h1[2]/h1.getLength();
  double tau1 = acos(n1.dot(e1) / n1.getLength() / e1.getLength()) * 180.0 / 3.14159265;
  if (e1[1] < 0) {
    tau1 = 360-tau1;
  }
  double ab1 = a1/b1;

  glPushMatrix();
  glColor3f(0, 1, 1);

  double eps1 = 1.0-(b1*b1/(a1*a1));
  if (eps1 > 0) {
    eps1 = sqrt(eps1);
  } else {
    eps1 = 0;
  }


  double maxDiameter1 = 2*a1*1e-6;
  double translateDist1 = eps1*a1*1e-6;
  double scaleFactor1 = a1/b1 * maxDiameter1 / 2.0;

  //Renderer::getInstance().textOut(0.5, 0.5, 0, "a: %lf, b: %lf, maxDiameter: %lf, translateDist: %lf, ScaleFactor: %lf", a*1e-6, b*1e-6, maxDiameter, translateDist, scaleFactor);

  double scaleFactorFin = (scaleFactor1 > scaleFactor)? scaleFactor1:scaleFactor;
  glColor3f(0.2, 0.7, 0.1);

  glScalef(0.7/scaleFactorFin, 0.7/scaleFactorFin, 0.7/scaleFactorFin);

  glBegin(GL_LINE_STRIP);
  glVertex3f(0,0,0);
  glVertex3f(r[0]*1e-6,-r[1]*1e-6,0);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(0,0,0);
  glVertex3f(r1[0]*1e-6,-r1[1]*1e-6,0);
  glEnd();

  glPushMatrix();
  static double plAng = 0;
  glRotatef(75, 1, 0, 0);
  glRotatef(plAng, 0, 0, 1);
  plAng += 0.05;
  if (plAng > 360) {
    plAng = 0;
  }
  glutWireSphere(gravityRef_->getRadius(), 15, 10);
  glPopMatrix();

  glPushMatrix();
  glRotatef(-tau - 90, 0, 0, 1);

  glTranslatef(0, -translateDist, 0);

  double multipleFactor = maxDiameter/2;
  double multipleFactor1 = maxDiameter1/2;


  glBegin(GL_LINE_STRIP);
  for (double i=0.0; i<361.0; i+=1.0) {
    double ang = i*3.14159265/180.0;
    glVertex3f(cos(ang)*multipleFactor/ab, sin(ang)*multipleFactor, 0);
  }
  glEnd();
  glPopMatrix();
  glRotatef(-tau1 - 90, 0, 0, 1);

  glTranslatef(0, -translateDist1, 0);
  glBegin(GL_LINE_STRIP);
  for (double i=0.0; i<361.0; i+=1.0) {
    double ang = i*3.14159265/180.0;
    glVertex3f(cos(ang)*multipleFactor1/ab1, sin(ang)*multipleFactor1, 0);
  }
  glEnd();


  glPopMatrix();
}


void NavDisplay::drawOrbit(Ship* ship)
{
  AstralBody* gravityRef = gravityRef_;
  Vector3 r = (ship->getCoord() - gravityRef->getCoord()) * 1e6;
  Vector3 v = (ship->getVelocity() - gravityRef->getVelocity());
  double M = gravityRef->getMass() * 1e6;
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

  double ab = a/b;

  glPushMatrix();
  glColor3f(0, 1, 1);

  double eps = 1.0-(b*b/(a*a));
  if (eps > 0) {
    eps = sqrt(eps);
  } else {
    eps = 0;
  }

  double T = 2*PI*sqrt(a*a*a/myu); //period in seconds

  double maxDiameter = 2*a*1e-6;
  double translateDist = eps*a*1e-6;
  double scaleFactor = a/b * maxDiameter / 2.0;

  dp_ = a*(1-eps) * 1e-6;
  da_ = a*(1+eps) * 1e-6;

  //Renderer::getInstance().textOut(0.5, 0.5, 0, "a: %lf, b: %lf, maxDiameter: %lf, translateDist: %lf, ScaleFactor: %lf", a*1e-6, b*1e-6, e[0], e[1], e[2]);
  glColor3f(0.2, 0.7, 0.1);

  glScalef(0.7/scaleFactor, 0.7/scaleFactor, 0.7/scaleFactor);


  glBegin(GL_LINE_STRIP);
    glVertex3f(0,0,0);
    glVertex3f(r[0]*1e-6,-r[1]*1e-6,0);
  glEnd();


  glPushMatrix();
  static double plAng = 0;

  if (gravityRef_->hasAtmosphere()) {
    double atm = gravityRef_->getAtmRadius();
    glColor3f(0.3,0.3,0.3);
    glBegin(GL_LINE_STRIP);
    for (double i=0.0; i<361.0; i+=1.0) {
      double ang = i*3.14159265/180.0;
      glVertex3f(cos(ang)*atm, sin(ang)*atm, 0);
    }
    glEnd();
    glColor3f(0.2, 0.7, 0.1);

  }


  glRotatef(75, 1, 0, 0);
  glRotatef(plAng, 0, 0, 1);
  plAng += 0.05;
  if (plAng > 360) {
    plAng = 0;
  }
  glutWireSphere(gravityRef_->getRadius(), 15, 10);
  glPopMatrix();

  double modifier = r.getNormalized().dot(v.getNormalized() * Vector3(0,0,1));
  if (modifier > 0) {
    glRotatef(-tau - 90, 0, 0, 1);
  } else {
    glRotatef(tau + 90, 0, 0, 1);
  }


  //dot(v0, cross(v1, v2))? 

  glTranslatef(0, -translateDist, 0);


  //glScalef(1, 1, 1);

  double multipleFactor = maxDiameter/2;

  glBegin(GL_LINE_STRIP);
  for (double i=0.0; i<361.0; i+=1.0) {
    double ang = i*3.14159265/180.0;
    glVertex3f(cos(ang)*multipleFactor/ab, sin(ang)*multipleFactor, 0);
  }
  glEnd();
  glTranslatef(0, multipleFactor, 0);
  glutWireSphere(0.5, 3, 3);


  glTranslatef(0, -2*multipleFactor, 0);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.5, -0.5, 0);
  glVertex3f(-0.5, 0.5, 0);
  glVertex3f(0.5, 0.5, 0);
  glVertex3f(0.5, -0.5, 0);
  glEnd();


  glPopMatrix();

}

void NavDisplay::drawDocking()
{
  if (!ship_ || !gravityRef_ || !(gravityRef_->getType() == Renderable::StationType)) {
    return;
  }
  Station* station = (Station*)gravityRef_;
  glPushMatrix();
  glRotatef(270, 1, 0, 0);
  glutWireCone(0.02, 0.1, 10, 10);
  glPopMatrix();

  Vector3 dir = station->getCoord() - ship_->getCoord();

  double dist = (dir.getLength()+0.5) * 10.0;
  if (dist > 10) {
    dist = 10.0;
  }
  double scaleF = dist;
  dir *= scaleF;
  double yaw = ship_->getYaw()*3.14159265/180.0;
  //glPushMatrix();
  glRotatef(ship_->getYaw(), 0, 0, 1);
  glTranslatef(dir[0], -dir[1], 0);
  glutWireSphere(0.05, 4, 2);
  //glPopMatrix();

  Vector3 stationPort = station->getDockingPort()*0.05;
  glRotatef(station->getYaw() + station->getPortAngle(), 0, 0, 1);
  
  glBegin(GL_LINES);
  glVertex3f(0,0,0);
  glVertex3f(-0.1,0,0);
  glEnd();
}

void NavDisplay::drawAxes()
{
  glPushMatrix();
  //Renderer::getInstance().getCamera().applyZoom();

  Vector3 relSpd = ship_->getVelocity() - gravityRef_->getVelocity();
  double spd = relSpd.getLength();
  relSpd.normalize();
  relSpd *= 0.5 * spd / 1000.0;
  
  glColor3f(0, 0.2, 0.9);
  glBegin(GL_LINES);
  glVertex3f(0,0,ship_->getRadius()*GLOBAL_MULT);
  glVertex3f(relSpd[0], -relSpd[1], 0);
  glEnd();

  Vector3 dir = gravityRef_->getCoord() - ship_->getCoord();
  double dist = dir.getLength();
  dir.normalize();
  dir *= /*ship_->getRadius() * */0.5 * sqrt(dist/10);
  
  glColor3f(0, 0.9, 0.2);
  glBegin(GL_LINES);
  glVertex3f(0,0,ship_->getRadius()*GLOBAL_MULT);
  glVertex3f(dir[0], -dir[1], 0);
  glEnd();

  Vector3 sDir = Vector3(sin(ship_->getYaw()*3.14159265/180.0)*0.3, -cos(ship_->getYaw()*3.14159265/180.0)*0.3, 0);
  //sDir *= ship_->getRadius()/* * (1/10000.0)*/;
  glColor3f(0.7, 0.9, 0.7);
  glBegin(GL_LINES);
  glVertex3f(0,0,ship_->getRadius()*GLOBAL_MULT);
  glVertex3f(sDir[0], -sDir[1], 0);
  glEnd();

  glPopMatrix();

}

void NavDisplay::render()
{
  WLayout::render();
  if (minimized_) {
    return;
  }
  updateGravityRef();
  glDisable(GL_LIGHTING);

  //double distance = sqrt((ship_->getCoord()[0]-ship_->gravityRef_->getCoord()[0])*(ship_->getCoord()[0]-ship_->gravityRef_->getCoord()[0]) + 
  //  (ship_->getCoord()[1]-ship_->gravityRef_->getCoord()[1])*(ship_->getCoord()[1]-ship_->gravityRef_->getCoord()[1]));
  //Vector3 relSpd = ship_->getVelocity() - ship_->gravityRef_->getVelocity();


  Vector3 direction = ship_->getCoord() -  gravityRef_->getCoord();
  direction.normalize();
  direction *= 0.1;

  glColor3f(1,1,1);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0.7, 0.9, 0);
  glVertex3f(0.9, 0.9, 0);
  glVertex3f(0.9, 0.7, 0);
  glVertex3f(0.7, 0.7, 0);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(0.8, 0.8, 0);
  glVertex3f(0.8 - direction[0], 0.8 + direction[1], 0);
  glEnd();

  if (mode_ == Orbit) {
    drawOrbit(ship_);
  } else if (mode_ == SyncOrbit) {
    if (syncOrbitRef_) {
      DynamicBody* dyn = (DynamicBody*)syncOrbitRef_;
      drawSyncOrbit(ship_, dyn);
    }
  } else if (mode_ == Docking) {
    drawDocking();
  } else if (mode_ == Axes) {
    drawAxes();
  }
  //glLoadIdentity();
  //vector<Vector3> points;
  //points.push_back(getCoord());
  //Vector3 point = ship_->getCoord() - ship_->gravityRef_->getCoord();
  //Vector3 velItr = ship_->getVelocity() - ship_->gravityRef_->getVelocity();
  //vector<Vector3> points;

  //double speedReduce = Gamani::getInstance().getSpeedReduce();

  //glColor3f(1,1,1);
  //glPushMatrix();
  //Vector3 drawP = (point - ship_->gravityRef_->getCoord()) * (0.3/ship_->gravityRef_->getRadius());
  //glTranslatef(drawP[0], drawP[1], drawP[2]);
  //glutSolidSphere(0.05, 10, 10);
  //glPopMatrix();
  //glColor3f(1,1,0);
  //double maxDist = 0.001;
  //double minDist = distance;

  //double mult = 50.0;
  //double speedReduce = Gamani::getInstance().getSpeedReduce();

  //for (int i=0; i<300; ++i) {
  //  Vector3 drawP = (point)/* * (0.3/gravityRef_->getRadius())*/;
  //  double dist = sqrt(point[0]*point[0] + point[1]*point[1]);
  //  mult = (dist - ship_->gravityRef_->getRadius() + 0.01) * 15 / speedReduce;
  //  //mult *= speedReduce;
  //  if (i%10 == 0) {
  //    points.push_back(drawP);
  //  }
  //  if (dist < ship_->gravityRef_->getRadius()) {
  //    break;
  //  }
  //  if (dist > maxDist) {
  //    maxDist = dist;
  //  }
  //  if (dist < minDist) {
  //    minDist = dist;
  //  }
  //  //glVertex3f(drawP[0], drawP[1], drawP[2]);


  //  Vector3 coordFrom = Vector3(0,0,0);
  //  Vector3 coordTo = point;
  //  double massFrom = ship_->gravityRef_->getMass() * 1e6; //kg
  //  double massTo = ship_->getMass() * 1e6; //kg
  //  //G*m*m/r^2
  //  static double G = 6.6725e-11;
  //  double distSquare = (coordFrom[0]-coordTo[0])*(coordFrom[0]-coordTo[0])+(coordFrom[1]-coordTo[1])*(coordFrom[1]-coordTo[1])+(coordFrom[2]-coordTo[2])*(coordFrom[2]-coordTo[2]);
  //  distSquare *= 1e6*1e6; //meters^2
  //  double force = G*massFrom*massTo*(1/distSquare); //kg^2/m^2*M*m^2/kg^2 = N
  //  //if (!body2->isStatic()) {
  //  Vector3 velocity = velItr * (mult *speedReduce);
  //  //velocity *= mult; //m/s
  //  Vector3 dir = coordFrom - coordTo;
  //  dir.normalize();
  //  //f = m*a; a=f/m; a=dv
  //  double dVelocity = force/massTo*mult*mult *speedReduce * speedReduce;
  //  velocity += Vector3(dir[0]*dVelocity, dir[1]*dVelocity, dir[2]*dVelocity);

  //  point *= 1.0e6;
  //  point += velocity;
  //  point *= 1.0/1.0e6;

  //  velItr = velocity * (1/mult/speedReduce);
  //}

  //glScalef(1/maxDist*0.7, 1/maxDist*0.7, 1/maxDist*0.7);

  //glPushMatrix();
  //static double plAng = 0;
  //glRotatef(75, 1, 0, 0);
  //glRotatef(plAng, 0, 0, 1);
  //plAng += 0.05;
  //if (plAng > 360) {
  //  plAng = 0;
  //}
  //glutWireSphere(ship_->gravityRef_->getRadius(), 15, 10);
  //glPopMatrix();

  //Renderer::getInstance().textOut(0.5, 0.4, 0, "maxdist_old: %lf", maxDist);

  //glBegin(GL_LINE_STRIP);
  //for (uint32_t i=0; i<points.size(); ++i) {
  //  glVertex3f(points[i][0], -points[i][1], 0);
  //}
  //glEnd();
}

void NavDisplay::updateGravityRef()
{
  if (manualRef_) {
    return;
  }
  vector<AstralBody*>& objects = *Gamani::getInstance().getWorld()->getAllObjects();
  AstralBody* newRef = NULL;
  double maxGrav = 0;
  Vector3 coordTo = ship_->getCoord();

  for (uint32_t i=0; i<objects.size(); ++i) {
    AstralBody* obj = objects[i];
    if (obj == ship_) {
      continue;
    }

    //if (obj->getName() == CString("Earth") || obj->getName() == CString("Shipyard")) {
    //  int aaa = 0;
    //}

    Vector3 coordFrom = obj->getCoord();
    double massFrom = obj->getMass() * 1e6; //kg
    double massTo = ship_->getMass() * 1e6; //kg
    //G*m*m/r^2
    static double G = 6.6725e-11;
    double distSquare = (coordFrom[0]-coordTo[0])*(coordFrom[0]-coordTo[0])+(coordFrom[1]-coordTo[1])*(coordFrom[1]-coordTo[1])+(coordFrom[2]-coordTo[2])*(coordFrom[2]-coordTo[2]);
    distSquare *= 1e6*1e6; //meters^2
    double force = G*massFrom*massTo/distSquare; //kg^2/m^2*M*m^2/kg^2 = N
    if (force > maxGrav) {
      maxGrav = force;
      newRef = obj;
    }
  }
  if (newRef) {
    gravityRef_ = newRef;
  } else {
    assert(0);
  }
}

void NavDisplay::toggleAutoRef()
{
  manualRef_ = false;
  //refIdx_ = -1;
}
