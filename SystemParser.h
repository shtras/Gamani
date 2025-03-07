#pragma once
#include "StarSystem.h"
#include "Ship.h"
#include "LayoutManager.h"

#define PARSER_VERSION 1.4

class Section;

class SystemParser
{
public:
  SystemParser();
  ~SystemParser();
  StarSystem* parseSystem(CString fileName, LayoutManager* layoutManager);
  Ship* getPlayerShip() {return controlledShip_;}
  const vector<AstralBody*>& getFreeObjects() const {return freeObjects_;}

  void dumpSystem(CString fileName);
private:
  void dumpBody(AstralBody* body);
  bool isComment(CString str);
  CString stripSpaces(CString str);
  bool readInfo();
  CString getNextLine();
  Section* getSection(CString name = "");
  void returnLine() {lineReturned_ = true;}
  bool isOpenSectionLine(CString line);
  bool isCloseSectionLine(CString line);
  bool verifyInfo();
  StarSystem* parseData();
  double getDouble(CString& str);
  bool parseSectionInfo(StarSystem* system, Section* section, AstralBody* upperLayer);
  bool fillAstralInfo(AstralBody* bodyToFill, Section* section, AstralBody* upperLayer);
  void calcAutoInfo(AstralBody* from, AstralBody* to, double dist, double angle);
  Vector3 parseVector(CString str);

  ifstream inputFile_;
  ofstream outputFile_;

  char buffer_[1024];
  bool lineReturned_;
  Section* infoSection_;
  Section* starSection_;

  vector<AstralBody*> freeObjects_;

  Ship* controlledShip_;
  LayoutManager* layoutManager_;
};

class Section
{
public:
  Section();
  ~Section();
  void addSection (Section* sec);
  void addLine (CString line);
  const vector<CString>& getLines() const {return lines_;}
  const vector<Section*>& getSections() const {return subSections_;}
  void setName(CString name) {name_ = name;}
  CString getName() const {return name_;}
private:
  CString name_;
  vector<CString> lines_;
  vector<Section*> subSections_;
};

