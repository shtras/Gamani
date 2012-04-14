#include "StdAfx.h"
#include "SystemParser.h"
#include <math.h>
#include "Planet.h"
#include "ModelRenderable.h"
#include "Dockable.h"
#include "Station.h"
#include "HUD.h"
#include "StationDisplay.h"
#include "Satellite.h"


SystemParser::SystemParser(void):lineReturned_(false),controlledShip_(NULL)
{
}


SystemParser::~SystemParser(void)
{
}

CString SystemParser::stripSpaces(CString str)
{
  str = str.remove(' ');
  return str.remove('\t');
}

bool SystemParser::isComment(CString str)
{
  if (str.getSize() >= 2 && str[0] == '/' && str[1] == '/') {
    return true;
  }
  return false;
}

CString SystemParser::getNextLine()
{
  if (lineReturned_) {
    lineReturned_ = false;
    CString res = CString(buffer_);
    return stripSpaces(res);
  }
  while (!file_.eof()) {
    file_.getline(buffer_, 1024);
    CString currString = CString(buffer_);
    currString = stripSpaces(currString);
    if (currString.getSize() == 0) {
      continue;
    }
    if (isComment(currString)) {
      continue;
    }
    return currString;
  }
  Logger::getInstance().log(ERROR_LOG_NAME, "Unexpected end of starsystem file. Looks like braces mismatch");
  return "";
}

bool SystemParser::isOpenSectionLine(CString line)
{
  return line.getIndex('{') != -1;
}

bool SystemParser::isCloseSectionLine(CString line)
{
  return line.getIndex('}') != -1;
}

Section* SystemParser::getSection(CString name/* = ""*/)
{
  CString header = getNextLine();
  int posBracket = header.getIndex('{');
  if (posBracket == -1) {
    return NULL;
  }
  CString sectionName = header.left(posBracket);
  if (!name.isEmpty() && sectionName != name) {
    return NULL;
  }
  Section* res = new Section();
  res->setName(sectionName);
  
  while (1) {
    CString nextLine = getNextLine();
    if (nextLine.isEmpty()) {
      delete res;
      return NULL;
    }
    if (isOpenSectionLine(nextLine)) {
      returnLine();
      Section* innerSection = getSection();
      res->addSection(innerSection);
      continue;
    }
    if (isCloseSectionLine(nextLine)) {
      break;
    }
    if (nextLine.getIndex(';') == -1) {
      Logger::getInstance().log(ERROR_LOG_NAME, "Cannot parse starsystem file. ';' not found at the end of line");
      delete res;
      return NULL;
    }
    nextLine = nextLine.left(nextLine.getSize() - 1); //Remove ;
    res->addLine(nextLine);
  }

  return res;
}

bool SystemParser::readInfo()
{
  infoSection_ = getSection("Info");
  if (!infoSection_) {
    Logger::getInstance().log(ERROR_LOG_NAME, "Cannot read info section from the starsystem file");
    return false;
  }
  starSection_ = getSection("Star");
  if (!starSection_) {
    Logger::getInstance().log(ERROR_LOG_NAME, "Cannot read star section from the starsystem file");
  }
  return true;
}

double SystemParser::getDouble(CString str)
{
  str = str.toLower();
  double res = 0;
  bool neg = false;
  int exp = 0;
  bool negativeExp = false;
  int decimalCnt = 1;
  bool afterDecimal = false;
  bool afterExp = false;
  for (int i=0; i<str.getSize(); ++i) {
    char c = str[i];
    int currdigit = 0;
    if (i == 0 && c == '-') {
      neg = true;
      continue;
    } else if (c == '.') {
      res /= 10;
      afterDecimal = true;
      continue;
    } else if (c == 'e') {
      if (str[i+1] == '-') {
        ++i;
        negativeExp = true;
      }
      if (!afterDecimal) {
        res /= 10;
      }
      afterExp = true;
    } else if (c >= '0' && i <= '9') {
      currdigit = c - '0';
    } else {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Wrong double value: ") + str);
      return 0;
    }
    if (afterExp) {
      exp += currdigit;
      exp *= 10;
      continue;
    }
    if (afterDecimal) {
      res += currdigit * pow(10.0, -decimalCnt);
      ++decimalCnt;
      continue;
    }
    res += currdigit;
    res *= 10;
  }
  if (!afterDecimal && !afterExp) {
    res /= 10;
  }
  if (negativeExp) {
    exp = -exp;
  }
  exp /= 10;
  res *= pow(10.0, exp);
  if (neg) {
    res = -res;
  }
  return res;
}

Vector3 SystemParser::parseVector(CString str)
{
  if (str[0] != '(' || str[str.getSize()-1] != ')') {
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Cannot parse vector string: ") + str);
    return Vector3();
  }
  str = str.substr(1, str.getSize() - 1);
  vector<CString> parts = str.tokenize(',');
  if (parts.size() != 3) {
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Cannot parse vector string: ") + str);
    return Vector3();
  }
  double x = getDouble(parts[0]);
  double y = getDouble(parts[1]);
  double z = getDouble(parts[2]);
  return Vector3(x,y,z);
}

void SystemParser::calcAutoInfo(AstralBody* from, AstralBody* to, double dist, double angle)
{
  Vector3 coordFrom = from->getCoord();
  Vector3 velFrom = from->getVelocity();
  double radAngle = DegToRad(angle);
  double radAngleVel = DegToRad(angle-90);
  Vector3 coordTo = coordFrom + Vector3(dist*cos(radAngle), dist*sin(radAngle), 0);
  double G = 6.6725e-11;
  double v1 = sqrt (G * from->getMass() / fabs(dist));
  Vector3 velTo = velFrom + Vector3(v1*cos(radAngleVel), v1*sin(radAngleVel), 0);
  to->setCoord(coordTo);
  to->setVelocity(velTo);
}

bool SystemParser::fillAstralInfo(AstralBody* bodyToFill, Section* section, AstralBody* upperLayer)
{
  bool autoInfo = false;
  double autoDist = 0;
  double autoAngle = 0;
  const vector<CString>& lines = section->getLines();
  for (unsigned int i=0; i<lines.size(); ++i) {
    CString str = lines[i];
    vector<CString> parts = str.tokenize('=');
    if (parts.size() != 2) {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Cannot parse line ") + str);
      return false;
    }
    CString field = parts[0].toLower();
    CString value = parts[1]/*.toLower()*/;
    if (field == "coord") {
      Vector3 valueVec = parseVector(value);
      bodyToFill->setCoord(valueVec);
    } else if (field == "radius") {
      bodyToFill->setRadius(getDouble(value));
    } else if (field == "mass") {
      bodyToFill->setMass(getDouble(value));
    } else if (field == "velocity") {
      bodyToFill->setVelocity(parseVector(value));
    } else if (field == "dist") {
      autoDist = getDouble(value);
      autoInfo = true;
    } else if (field == "angle") {
      autoAngle = getDouble(value);
      autoInfo = true;
    } else if (field == "name") {
      bodyToFill->setName(value);
    } else if (field == "rotationperiod") {
      bodyToFill->setRotationPeriod(getDouble(value));
    } else if (field == "color") {
      bodyToFill->setColor(parseVector(value));
    } else if (field == "atmradius") {
      if (bodyToFill->getType() != AstralBody::PlanetType) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Wrong parameter in starsystem file: only planets can have atmosphere");
        return false;
      }
      Planet* p = static_cast<Planet*>(bodyToFill);
      p->setAtmRadius(getDouble(value));
    } else if (field == "atmcolor") {
      if (bodyToFill->getType() != AstralBody::PlanetType) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Wrong parameter in starsystem file: only planets can have atmosphere");
        return false;
      }
      Planet* p = static_cast<Planet*>(bodyToFill);
      p->setAtmColor(parseVector(value));
    } else if (field == "model") {
      ModelRenderable* modelRen = dynamic_cast<ModelRenderable*>(bodyToFill);
      if (!modelRen) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Cannot assign model to object");
        return false;
      }
      modelRen->initModel(value);
    } else if (field == "dockingport") {
      Dockable* dockable = dynamic_cast<Dockable*>(bodyToFill);
      if (!dockable) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Cannot assign docking port to object");
        return false;
      }
      dockable->setPortDist(getDouble(value));
    } else if (field == "portangle") {
      Dockable* dockable = dynamic_cast<Dockable*>(bodyToFill);
      if (!dockable) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Cannot assign docking port to object");
        return false;
      }
      dockable->setPortAngle(getDouble(value));
    } else if (field == "yawpower") {
      if (bodyToFill->getType() != AstralBody::ShipType) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Error parsing starsystem file. Can assign yaw power to ships only");
        return false;
      }
      Ship* ship = static_cast<Ship*>(bodyToFill);
      ship->setYawPower(getDouble(value));
    } else if (field == "playercontrolled") {
      if (bodyToFill->getType() != AstralBody::ShipType) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Error parsing starsystem file. Only ships can have playercontrolled attribute");
        return false;
      }
      Ship* ship = static_cast<Ship*>(bodyToFill);
      if (value.toLower() == "true") {
        ship->initializeAsPlayerControlled();
      }
    } else {
      Logger::getInstance().log(ERROR_LOG_NAME, CString("Wrong field: ") + field);
      return false;
    }
  }
  if (autoInfo) {
    if (!upperLayer) {
      bodyToFill->setCoord(Vector3(0,0,0));
      bodyToFill->setVelocity(Vector3(0,0,0));
    } else {
      calcAutoInfo(upperLayer, bodyToFill, autoDist, autoAngle);
    }
  }
  return true;
}

bool SystemParser::parseSectionInfo(StarSystem* system, Section* section, AstralBody* upperLayer)
{
  CString name = section->getName();
  name = name.toLower();
  AstralBody* currBody = NULL;
  if (name == "star") {
    assert (!upperLayer);
    currBody = new Star();
    if (!fillAstralInfo(currBody, section, upperLayer)) {
      return false;
    }
    system->addStar(static_cast<Star*>(currBody));
  } else if (name == "planet") {
    currBody = new Planet();
    if (!fillAstralInfo(currBody, section, upperLayer)) {
      return false;
    }
    upperLayer->addSatellite(currBody);
  } else if (name == "station") {
    currBody = new Station();
    if (!fillAstralInfo(currBody, section, upperLayer)) {
      return false;
    }
    //StationDisplay* stationDisplay = new StationDisplay();
    //stationDisplay->init();
    //stationDisplay->setVisible(false);
    //layoutManager_->addLayout(stationDisplay);
    upperLayer->addSatellite(currBody);
    //(static_cast<Station*>(currBody))->setDisplay(stationDisplay);
  } else if (name == "satellite") {
    currBody = new Satellite();
    if (!fillAstralInfo(currBody, section, upperLayer)) {
      return false;
    }
    upperLayer->addSatellite(currBody);
  } else if (name == "ship") {
    currBody = new Ship();
    if (!fillAstralInfo(currBody, section, upperLayer)) {
      return false;
    }
    HUD* shipHud = new HUD();
    shipHud->AddLayout.connect(layoutManager_, &LayoutManager::addLayout);
    shipHud->RemoveLayout.connect(layoutManager_, &LayoutManager::removeLayout);
    shipHud->init(static_cast<Ship*>(currBody));
    if ((static_cast<Ship*>(currBody))->isInitializedAsPlayerControlled()) {
      controlledShip_ = static_cast<Ship*>(currBody);
      shipHud->setVisible(true);
    } else {
      shipHud->setVisible(false);
    }
    freeObjects_.push_back(currBody);
  } else {
    Logger::getInstance().log(ERROR_LOG_NAME, CString("Error parsing starsystem file. Unknown section type: ") + name);
    return false;
  }

  const vector<Section*>& subSections = section->getSections();
  for (unsigned int i=0; i<subSections.size(); ++i) {
    if (!parseSectionInfo(system, subSections[i], currBody)) {
      return false;
    }
  }
  return true;
}

StarSystem* SystemParser::parseData()
{
  StarSystem* system = new StarSystem();
  bool res = parseSectionInfo(system, starSection_, NULL);
  if (!res) {
    Logger::getInstance().log(ERROR_LOG_NAME, "Parsing starsystem file failed");
    return NULL;
  }
  return system;
}

bool SystemParser::verifyInfo()
{
  for (unsigned int i=0; i<infoSection_->getLines().size(); ++i) {
    CString line = infoSection_->getLines()[i];
    vector<CString> parts = line.tokenize('=');
    if (parts.size() != 2) {
      return false;
    }
    CString field = parts[0].toLower();
    CString value = parts[1].toLower();
    if (field == CString("version")) {
      double version = getDouble(value);
      if (version != (PARSER_VERSION)) {
        Logger::getInstance().log(ERROR_LOG_NAME, "Wrong starsystem file version");
        return false;
      }
    }
  }
  return true;
}

StarSystem* SystemParser::parseSystem(CString fileName, LayoutManager* layoutManager)
{
  layoutManager_ = layoutManager;
  file_.open(fileName);
  bool res = readInfo();
  if (!res) {
    file_.close();
    return NULL;
  }
  file_.close();

  res = verifyInfo();
  if (!res) {
    return NULL;
  }

  StarSystem* system = parseData();

  return system;
}


Section::Section()
{

}

Section::~Section()
{
  for (unsigned int i=0; i<subSections_.size(); ++i) {
    delete subSections_[i];
  }
}

void Section::addSection(Section* sec)
{
  subSections_.push_back(sec);
}

void Section::addLine(CString line)
{
  lines_.push_back(line);
}
