#include "StdAfx.h"
#include "NavDisplay.h"
#include "Ship.h"
#include "Gamani.h"
#include "Renderer.h"

NavDisplay::NavDisplay():syncOrbitRef_(NULL), mode_(Orbit)
{
}

NavDisplay::~NavDisplay()
{
}

void NavDisplay::init()
{
  setDimensions(0, 300, 300, 300);
  shipNameText_ = new WText();
  shipNameText_->setDimensions(10, 290, 100, 10);
  shipNameText_->setText("Hello!");
  addWidget(shipNameText_);

  modeName_ = new WText();
  modeName_->setDimensions(150, 290, 100, 10);
  modeName_->setText("Orbit mode");
  addWidget(modeName_);

  gravityRefName_ = new WText();
  gravityRefName_->setDimensions(10, 275, 100, 10);
  gravityRefName_->setText("Hello!");
  addWidget(gravityRefName_);

  velData_ = new WText();
  velData_->setDimensions(10, 260, 100, 10);
  velData_->setText("AAA!");
  addWidget(velData_);

  modeButton_ = new WButton();
  modeButton_->setDimensions(75, 30, 70, 29);
  modeButton_->sigClick.connect(this, &NavDisplay::modeButtonClick);
  modeButton_->setLabel("Mode");
  addWidget(modeButton_);

  tgtToSyncOrbitButton_ = new WButton();
  tgtToSyncOrbitButton_->setDimensions(150, 30, 70, 29);
  tgtToSyncOrbitButton_->sigClick.connect(this, &NavDisplay::tgtToSyncOrbitButtonClick);
  tgtToSyncOrbitButton_->setLabel("Target");
  tgtToSyncOrbitButton_->setVisible(false);
  addWidget(tgtToSyncOrbitButton_);

  modeMenu_ = new WMenu();
  modeMenu_->setDimensions(5, 5, 100, 290);
  /////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode1Item = new WMenu::Item();
  mode1Item->setDimensions(6,288,98,20);
  mode1Item->setLabel("Orbit");
  modeMenu_->addItem(mode1Item);
  mode1Item->setValue((void*)1);
  mode1Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode1Item);
  //////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode2Item = new WMenu::Item();
  mode2Item->setDimensions(6,266,98,20);
  mode2Item->setLabel("Sync orbit");
  modeMenu_->addItem(mode2Item);
  mode2Item->setValue((void*)2);
  mode2Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode2Item);
  //////////////////////////////////////////////////////////////////////////
  WMenu::Item* mode3Item = new WMenu::Item();
  mode3Item->setDimensions(6,244,98,20);
  mode3Item->setLabel("Landing");
  modeMenu_->addItem(mode3Item);
  mode3Item->setValue((void*)3);
  mode3Item->sigClick.connect(this, &NavDisplay::menuModeClick);
  addWidget(mode3Item);
  //////////////////////////////////////////////////////////////////////////
  modeMenu_->setVisible(false);
  addWidget(modeMenu_);

  tgtSelectMenu_ = new WMenu();
  tgtSelectMenu_->setDimensions(150, 30, 70, 29);
  tgtSelectMenu_->setVisible(false);
  addWidget(tgtSelectMenu_);

  visible_ = true;
}

void NavDisplay::setTarget(void* tgt)
{
  tgtSelectMenu_->setVisible(false);
  AstralBody* newTgt = (AstralBody*)tgt;
  syncOrbitRef_ = newTgt;
  tgtSelectMenu_->initialClear(this);
  addWidgetToGC(tgtSelectMenu_);
}

void NavDisplay::selectTargetStartingFrom(void* bodyRef)
{
  AstralBody* body = (AstralBody*)bodyRef;
  if (!body) {
    body = *Gamani::getInstance().getWorld()->getCurrentSystem()->getStars().begin();
  }
  tgtSelectMenu_->initialClear(this);

  WMenu::Item* mainItem = new WMenu::Item();
  mainItem->setDimensions(6,288,98,20);
  mainItem->setValue((void*)body);
  mainItem->setLabel(body->getName());
  mainItem->sigClick.connect(this, &NavDisplay::setTarget);
  tgtSelectMenu_->addItem(mainItem);
  addWidget(mainItem);

  vector<AstralBody*>& satellites = body->getSatellites();
  for (unsigned int i=0; i<satellites.size(); ++i) {
    AstralBody* satellite = satellites[i];

    WMenu::Item* item = new WMenu::Item();
    item->setDimensions(6, 266 - i*22, 98, 20);
    item->setValue((void*)satellite);
    item->setLabel(satellite->getName());
    item->sigClick.connect(this, &NavDisplay::selectTargetStartingFrom);
    tgtSelectMenu_->addItem(item);
    addWidget(item);
  }

  tgtSelectMenu_->setVisible(true);
}

void NavDisplay::tgtToSyncOrbitButtonClick()
{
  selectTargetStartingFrom(NULL);
}

void NavDisplay::modeButtonClick()
{
  modeMenu_->setVisible(true);
  shipNameText_->setText("World!");
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
    modeName_->setText("Landing mode");
    tgtToSyncOrbitButton_->setVisible(false);
    modeMenu_->setVisible(false);
    mode_ = Landing;
    break;
  }
}

void NavDisplay::updateData()
{
  shipNameText_->setText(ship_->getName());
  gravityRefName_->setText(ship_->getGravityRef()->getName());
  Vector3 relSpd = ship_->getVelocity() - ship_->getGravityRef()->getVelocity();
  Vector3 relPos = ship_->getCoord() - ship_->getGravityRef()->getCoord();
  double dist = relPos.getLength();
  char sprStr[100];
  sprintf(sprStr, "Rel spd: %0.2f: %0.2f Dist: %s", relSpd[0], relSpd[1], Renderer::getInstance().formatDistance(dist).operator const char *());

  velData_->setText(sprStr);
}

void NavDisplay::drawSyncOrbit(Ship* ship, DynamicBody* ref)
{
  Vector3 r = (ship->getCoord() - ship->getGravityRef()->getCoord()) * 1e6;
  Vector3 v = (ship->getVelocity() - ship->getGravityRef()->getVelocity());
  double M = ship->getGravityRef()->getMass() * 1e6;
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

  Vector3 r1 = (ref->getCoord() - ship->getGravityRef()->getCoord()) * 1e6;
  Vector3 v1 = (ref->getVelocity() - ship->getGravityRef()->getVelocity());
  double M1 = ship->getGravityRef()->getMass() * 1e6;
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
  glutWireSphere(ship_->gravityRef_->getRadius(), 15, 10);
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
  Vector3 r = (ship->getCoord() - ship->getGravityRef()->getCoord()) * 1e6;
  Vector3 v = (ship->getVelocity() - ship->getGravityRef()->getVelocity());
  double M = ship->getGravityRef()->getMass() * 1e6;
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

  glPushMatrix();
  glColor3f(0, 1, 1);

  double eps = 1.0-(b*b/(a*a));
  if (eps > 0) {
    eps = sqrt(eps);
  } else {
    eps = 0;
  }

  double maxDiameter = 2*a*1e-6;
  double translateDist = eps*a*1e-6;
  double scaleFactor = a/b * maxDiameter / 2.0;

  //Renderer::getInstance().textOut(0.5, 0.5, 0, "a: %lf, b: %lf, maxDiameter: %lf, translateDist: %lf, ScaleFactor: %lf", a*1e-6, b*1e-6, e[0], e[1], e[2]);
  glColor3f(0.2, 0.7, 0.1);

  glScalef(0.7/scaleFactor, 0.7/scaleFactor, 0.7/scaleFactor);


  glBegin(GL_LINE_STRIP);
    glVertex3f(0,0,0);
    glVertex3f(r[0]*1e-6,-r[1]*1e-6,0);
  glEnd();


  glPushMatrix();
  static double plAng = 0;
  glRotatef(75, 1, 0, 0);
  glRotatef(plAng, 0, 0, 1);
  plAng += 0.05;
  if (plAng > 360) {
    plAng = 0;
  }
  glutWireSphere(ship_->gravityRef_->getRadius(), 15, 10);
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


  glPopMatrix();

}

void NavDisplay::render()
{
  WLayout::render();
  ship_->updateGravityRef();
  glDisable(GL_LIGHTING);

  //double distance = sqrt((ship_->getCoord()[0]-ship_->gravityRef_->getCoord()[0])*(ship_->getCoord()[0]-ship_->gravityRef_->getCoord()[0]) + 
  //  (ship_->getCoord()[1]-ship_->gravityRef_->getCoord()[1])*(ship_->getCoord()[1]-ship_->gravityRef_->getCoord()[1]));
  //Vector3 relSpd = ship_->getVelocity() - ship_->gravityRef_->getVelocity();


  Vector3 direction = ship_->getCoord() -  ship_->gravityRef_->getCoord();
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
    DynamicBody* dyn = dynamic_cast<DynamicBody*>(syncOrbitRef_);
    if (dyn) {
      drawSyncOrbit(ship_, dyn);
    }
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
