#pragma once
#include "World.h"
#include "LayoutManager.h"
#include "UpperPanel.h"
#include "MainMenu.h"

class Gamani
{
public:
  enum State {MenuState, GameState, QuitState};

  static Gamani& getInstance();
  bool init(HINSTANCE& hIhstance);
  bool run();
  void handleMessage(UINT message,WPARAM wParam,LPARAM lParam);
  const double getSpeed() const {return speed_;}
  Renderable* getFollowedObject() {return world_->getFollowedObject();}
  bool isPaused() {return paused_;}
  World* getWorld() {return  world_;}
  double long getSeconds() {return seconds_;}
  double getSpeedReduce() {return calcStepLength_;}
  double getDTModifier() {return dtModifier_;}
  bool getAuxAxes() {return auxAxes_;}
  bool getTracers() {return tracers_;}
  bool getAuxPrints() {return auxPrint_;}
  bool getDrawNames() {return names_;}
  bool drawInterface() {return interface_;}

  void speedUp();
  void speedDown();
  void pause();
  bool usingSkyBox1() {return skybox1_;}
  void setSpeed1x() {speed_ = 1;}
  bool drawRelativeOrbits() {return relativeOrbits_;}
  bool rotateCameraWithObject() {return rotateCameraWithObject_;}
  bool shiftPressed() {return shiftPressed_;}
  void toggleTracers() {tracers_ = !tracers_;}
  void toggleNames() {names_ = !names_;}
  void toggleGUI() {interface_ = !interface_;}
  GLenum getDrawingMode() {return drawingMode_;}
  void switchDrawingMode();
  bool isPressed(int key);
  GLuint getShader() {return shader_;}
  bool drawBoundBoxes() {return drawBoundBoxes_;}
  void setNextState(State nextState) {nextState_ = nextState;}
  State getNextState() {return nextState_;}
  const CString& getVersion() {return version_;}
  void saveSystem();
  void setFileToLoad(CString fileName) {fileToload_ = fileName;}
  bool printASM() {return printASM_;}
private:
  bool testInit();
  void layoutTest();
  void handlePressedKey(int key);
//public:
  void handlePressedKeys();
private:
  Gamani();
  ~Gamani();
  bool mainLoop();
  bool menuLoop();
  bool startMenu();
  bool endMenu();
  bool startGame();
  bool endGame();
  void resetState();
  set<int> pressedKeys_;
  set<int> nonContKeys_;
  World* world_;
  bool paused_;
  double speed_;
  double long seconds_;
  double calcStepLength_;
  double dtModifier_;
  bool shiftPressed_;

  bool tracers_;
  bool auxPrint_;
  bool interface_;

  bool lmDown_;
  bool rmDown_;
  bool lmDrag_;
  bool rmDrag_;
  int lmCoord_[2];
  int rmCoord_[2];

  LayoutManager layoutManager_;
  bool auxAxes_;
  bool names_;

  UpperPanel* upperPanel_;
  MainMenu* mainMenu_;
  bool skybox1_;
  bool relativeOrbits_;

  bool rotateCameraWithObject_;

  GLenum drawingMode_;
  GLuint shader_;
  bool drawBoundBoxes_;

  State nextState_;
  CString version_;

  CString fileToload_;
  bool printASM_;
};

