#include "StdAfx.h"
#include "Gamani.h"
#include "Renderer.h"
#include "Ship.h"
#include "AstralBody.h"
#include "Star.h"
#include "Planet.h"
#include "WLayout.h"
#include "NavDisplay.h"
#include "HUD.h"
#include "VersionInfo.h"

#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif //DEBUG
#include "Station.h"
//#include "StationDisplay.h"
#include "MissionManager.h"
#include "SystemParser.h"
#include "SphereVBO.h"
#include "ADPCompiler.h"
#include "ADPFactory.h"

const char* Version = "0.4.0";

//#define MULTITHREAD_RUN

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

#ifdef MULTITHREAD_RUN
int IterationsToProcess = 0;
bool StopThread = false;
HANDLE ghWriteEvent;

DWORD WINAPI WorldCalcThread(LPVOID lpParam)
{
  World* world = (World*)lpParam;
  while(!StopThread) {
    WaitForSingleObject(ghWriteEvent, INFINITE);
    cout << IterationsToProcess << " ";
    int currentIters = IterationsToProcess;
    for (int i=0; i<currentIters; ++i) {
      world->interactionStep();
      Gamani::getInstance().handlePressedKeys();
    }
    IterationsToProcess -= currentIters;
    if (IterationsToProcess <= 0) {
      IterationsToProcess = 0;
      ResetEvent(ghWriteEvent);
    }
  }
  return 0;
}
#endif

void setVSync(int interval=1)
{
  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

  if( wglSwapIntervalEXT ) {
    wglSwapIntervalEXT(interval);
  }
}

void toggleVSync()
{
  static int interval = 1;
  setVSync(interval);
  if (interval == 0) {
    interval = 1;
  } else {
    interval = 0;
  }
}

Gamani::Gamani():world_(NULL), paused_(true), speed_(1), calcStepLength_(0.05), dtModifier_(50),auxAxes_(false),lmDown_(false),rmDown_(false),
  lmDrag_(false), rmDrag_(false), tracers_(false), auxPrint_(true), interface_(true), names_(false),skybox1_(false),relativeOrbits_(false),
  rotateCameraWithObject_(false),shiftPressed_(false),drawingMode_(GL_TRIANGLES),shader_(-1),drawBoundBoxes_(false), nextState_(MenuState),
  printASM_(false)
{
  nonContKeys_.insert('M');
  nonContKeys_.insert('V');
  nonContKeys_.insert('P');
  nonContKeys_.insert('Y');
  nonContKeys_.insert('U');
  nonContKeys_.insert('N');
  nonContKeys_.insert('F');
  nonContKeys_.insert('L');
  nonContKeys_.insert('X');
  nonContKeys_.insert('C');
  nonContKeys_.insert('Z');
  //nonContKeys_.insert('Q');
  //nonContKeys_.insert('E');
  nonContKeys_.insert('O');
  nonContKeys_.insert('R');
  nonContKeys_.insert('K');
  nonContKeys_.insert(0x30);
  nonContKeys_.insert(0x31);
  nonContKeys_.insert(0x32);
  nonContKeys_.insert(0x33);
  nonContKeys_.insert(0x34);
  nonContKeys_.insert(0x35);
  nonContKeys_.insert(0x39);
  nonContKeys_.insert(0x36);
  nonContKeys_.insert(0x37);
  nonContKeys_.insert(0xdd); //']'

  version_ = CString(Version) + "." + CString(BUILD_NUM) + " " + CString(BUILDSTR);
  fileToload_ = "system.txt";
}



Gamani::~Gamani()
{
  delete world_;
}

Gamani& Gamani::getInstance()
{
  static Gamani instance;
  return instance;
}

bool Gamani::init(HINSTANCE& hIhstance)
{
  Renderer& renderer = Renderer::getInstance();
  renderer.init(hIhstance);
  layoutManager_.setRenderer(new GUIRenderer());
  return true;
}

void Gamani::resetState()
{
  paused_ = true;
  speed_ = 1;
  calcStepLength_ = 0.05;
  dtModifier_ = 50;
  auxAxes_ = false;
  lmDown_ = false;
  rmDown_ = false;
  lmDrag_ = false;
  rmDrag_ = false;
  tracers_ = false;
  auxPrint_ = true;
  interface_ = true;
  names_ = false;
  skybox1_ = false;
  relativeOrbits_ = false;
  rotateCameraWithObject_ = false;
  shiftPressed_ = false;
  drawingMode_ = GL_TRIANGLES;
  shader_ = -1;
  drawBoundBoxes_ = false;
  nonContKeys_.insert('M');
  nonContKeys_.insert('V');
  nonContKeys_.insert('P');
  nonContKeys_.insert('Y');
  nonContKeys_.insert('U');
  nonContKeys_.insert('N');
  nonContKeys_.insert('F');
  nonContKeys_.insert('L');
  nonContKeys_.insert('X');
  nonContKeys_.insert('C');
  nonContKeys_.insert('Z');
  //nonContKeys_.insert('Q');
  //nonContKeys_.insert('E');
  nonContKeys_.insert('O');
  nonContKeys_.insert('R');
  nonContKeys_.insert('K');
  nonContKeys_.insert(0x30);
  nonContKeys_.insert(0x31);
  nonContKeys_.insert(0x32);
  nonContKeys_.insert(0x33);
  nonContKeys_.insert(0x34);
  nonContKeys_.insert(0x35);
  nonContKeys_.insert(0x39);
  nonContKeys_.insert(0x36);
  nonContKeys_.insert(0x37);
  nonContKeys_.insert(0xdd); //']'
  nonContKeys_.insert('B');
}

bool Gamani::startMenu()
{
  resetState();
  layoutManager_.reset();
  world_ = new World();

  Star* sun = new Star();
  sun->setMass(0.1);
  sun->setCoord(Vector3(10000, 0, 0));

  StarSystem* system = new StarSystem();
  system->addStar(sun);
  sun->setName("Sun");
  sun->setRadius(695.5);

  Planet* planet = new Planet();
  planet->setMass(0.1);
  planet->setCoord(Vector3(0,0,0));
  planet->setName("Earth");
  planet->setRadius(6.371);
  planet->setRotationPeriod(500);
  sun->addSatellite(planet);
  world_->setStarSystem(system);
  world_->switchFollowedObject(planet);
  Renderer::getInstance().getCamera().position(planet);
  Renderer::getInstance().getCamera().zoom(500);
  Renderer::getInstance().getCamera().setPitch(20);
  Renderer::getInstance().getCamera().setHeading(310);

  Renderer& renderer = Renderer::getInstance();
  renderer.changeRenderList(world_->getAllObjects());

  mainMenu_ = new MainMenu();
  mainMenu_->init();
  mainMenu_->setMinimizible(false);
  mainMenu_->setDimensions(0.02, 0.9, 0.25, 0.4);
  layoutManager_.addLayout(mainMenu_);

  return menuLoop();
}

bool Gamani::endMenu()
{
  delete world_;
  world_ = NULL;
  return true;
}

void Gamani::saveSystem()
{
  SystemParser parser;
  parser.dumpSystem("save.txt");
}

bool Gamani::startGame()
{
  resetState();
  MissionManager::getInstance().reset();

  Renderer::getInstance().getCamera().setPitch(10);
  Renderer::getInstance().getCamera().setHeading(180);

  layoutManager_.reset();
  world_ = new World();
  if (!testInit()) {
    return false;
  }
  Renderer& renderer = Renderer::getInstance();
  renderer.changeRenderList(world_->getAllObjects());
  bool res = mainLoop();
  return res;
}

bool Gamani::endGame()
{
  Renderer::getInstance().getParticleManager()->reset();
  delete world_;
  world_ = NULL;
  return true;
}

bool Gamani::menuLoop()
{
  setVSync(0);

  MSG msg={0};
  long timebase=GetTickCount();
  long currentTime = GetTickCount();
  long time = 0;
  double delta = 0;
  double accum = 0;
  int frame = 0;
  while(WM_QUIT!=msg.message) {
    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      Sleep(20);
      if (nextState_ != MenuState) {
        return true;
      }
      ++frame;
      time = GetTickCount();
      delta = time - currentTime;
      accum += delta;
      while (accum >= 50) {
        world_->interactionStep();
        accum -= 50;
      }
      currentTime = time;

      if (time - timebase > 1000) {
        double fps = frame*1000.0/(time-timebase);
        timebase = time;
        frame = 0;
        char cfps[200];
        sprintf(cfps, "%s.%d %s FPS: %.lf", Version, BUILD_NUM, BUILDSTR, fps);
        SetWindowTextA(Renderer::getInstance().getHwnd(), (LPCSTR)(cfps));
      }

      Renderer::getInstance().render();
      layoutManager_.render();
      Renderer::getInstance().renderEnd();
      checkReleaseError("Main cycle OpenGL error");
    }
  }
  nextState_ = QuitState;
  return true;
}

bool Gamani::mainLoop()
{
  //ADPCompiler compiler;
  //char memory[32000];
  //compiler.compile("res/asm/killrot.asm", memory, 32000);
  //int lastAddr = compiler.getLastAddr();
  //ofstream dumpFile("dump1.txt");
  //for (int i=0; i<lastAddr / 8; ++i) {
  //  if (i%8 == 0) {
  //    //dumpFile << endl;
  //  }
  //  for (int j=0; j<8; ++j) {
  //    if (j%8 == 0) {
  //      //dumpFile << " ";
  //    }
  //    char bit = getBit(memory[i], j);
  //    //dumpFile << (char)(bit + '0');
  //  }
  //  dumpFile << memory[i];
  //}
  //dumpFile.close();

  //int offset = 0;
  //int testAddr = 0;
  //vector<Instruction*> instrs;
  //while(1) {
  //  Instruction* instr = ADPFactory::getInstance().createInstr(memory, offset);
  //  if (!instr) {
  //    break;
  //  }
  //  cout << hex << testAddr/8 << " " << instr->toString() << endl;
  //  instrs.push_back(instr);
  //  testAddr += instr->getSize();
  //  assert (testAddr == offset);
  //  if (offset >= lastAddr) {
  //    break;
  //  }
  //}

  //char memory1[32000];
  //int rs[8];
  //for (int i=0; i<8; ++i) {
  //  rs[i] = i;
  //}
  //double fs[8];
  //int psw;
  //auto itr = instrs.begin();
  //Instruction* instr1 = *(itr++);
  //Instruction* instr2 = *(itr++);
  //Instruction* instr3 = *(itr++);
  //Instruction* instr4 = *(itr++);
  //instr1->emit(memory1, rs, fs, psw);
  //instr2->emit(memory1, rs, fs, psw);
  //instr3->emit(memory1, rs, fs, psw);
  //instr4->emit(memory1, rs, fs, psw);
  //int aaa = 0;





  //setVSync(0);
  int frame = 0;
  long timebase=GetTickCount();
  long currentTime = GetTickCount();
  long time;

  double accumulator = 0.0f;
  double dt = 1.0f;

  double accumKeys = 0.0f;
  double dtKeys = 1;

  seconds_ = 0;

  world_->selectShip();

  upperPanel_ = new UpperPanel();
  upperPanel_->init();
  upperPanel_->setMinimizible(false);
  layoutManager_.addLayout(upperPanel_);

  AstralBody* station = world_->getObject("Shipyard");
  assert (station && station->getType() == Renderable::StationType);
  MissionManager::getInstance().init(&layoutManager_);
  MissionManager::getInstance().testInit(static_cast<Station*>(station));

  setShaders("Shaders/TexItems.vert", "Shaders/TexItems.frag", &shader_);

  SphereVBO::getInstance().initVBO();
  int snapshotTimer = 0;
  MSG msg={0};
  world_->snapshot();
  double lastDelta = 0;
  double delta = 0;
  int cntToPause = 0;
#ifdef MULTITHREAD_RUN
  DWORD result;
  LPVOID param = (LPVOID)world_;
  ghWriteEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("WriteEvent")); 
  CreateThread(NULL, 0, WorldCalcThread, param, 0, &result);
#endif
  while(WM_QUIT!=msg.message) {
    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      if (nextState_ != GameState) {
        return true;
      }
      time = GetTickCount();
      ++frame;
      lastDelta = delta;
      delta = time - currentTime;
      if ((delta > lastDelta)) {
        cntToPause++;
        if (cntToPause > 3) {
          paused_ = true;
          cntToPause = 0;
        }
      } else {
        cntToPause = 0;
      }
      if (delta > 100) {
        paused_ = true;
      }
      currentTime = time;
      accumulator += delta;
      accumKeys += delta;

      if (speed_ < 1) {
        dt = 1.0/speed_;
      } else {
        dt = 1.0;
      }

      dt *= dtModifier_;
      if (accumulator < dt) {
        Sleep(5);
      }
      while(accumulator >= dt) {
        if (!paused_) {
#ifdef MULTITHREAD_RUN
          IterationsToProcess += speed_;
          SetEvent(ghWriteEvent);
#endif
          for (int i=0; i<speed_; ++i) {
            seconds_ += calcStepLength_*dt * (100/dtModifier_);
#ifndef MULTITHREAD_RUN
            world_->interactionStep();
            //for (int k=0; k<=1000; ++k) {
            //  instr1->emit(memory1, rs, fs, psw);
            //}

            handlePressedKeys();
#endif
            if (++snapshotTimer > 10000) {
              world_->snapshot();
              snapshotTimer = 0;
            }
            MissionManager::getInstance().checkCompleted();
          }
        }
        accumulator -= dt;
      }

      if (time - timebase > 1000) {
        double fps = frame*1000.0/(time-timebase);
        timebase = time;
        frame = 0;
        char cfps[200];
        sprintf(cfps, "%s.%d %s FPS: %.lf %s %lf", Version, BUILD_NUM, BUILDSTR, fps, (paused_)?"Paused":"Running", speed_);
        SetWindowTextA(Renderer::getInstance().getHwnd(), (LPCSTR)(cfps));
      }

      Renderer::getInstance().render();
      if (interface_) {
        layoutManager_.render();
      }
      Renderer::getInstance().renderEnd();
      checkReleaseError("Main cycle OpenGL error");
    }
  }
#ifdef MULTITHREAD_RUN
  StopThread = true;
  CloseHandle(ghWriteEvent);
#endif
  nextState_ = QuitState;
  return true;

}

void Gamani::handlePressedKey(int key)
{
  if (layoutManager_.focusGrabbed() && interface_) {
    if (layoutManager_.handlePressedKey(key)) {
      return;
    }
  }
  switch (key) {
  case VK_LEFT:
    Renderer::getInstance().getCamera().move(-0.1, 0, 1);
    break;
  case VK_RIGHT:
    Renderer::getInstance().getCamera().move(0.1, 0, 1);
    break;
  case VK_UP:
    Renderer::getInstance().getCamera().move(0, 0.1, 1);
    break;
  case VK_DOWN:
    Renderer::getInstance().getCamera().move(0, -0.1, 1);
    break;
  case VK_PRIOR:
    Renderer::getInstance().getCamera().rotate(0,0.1,0);
    break;
  case VK_NEXT:
    Renderer::getInstance().getCamera().rotate(0,-0.1,0);
    break;
//   case VK_OEM_COMMA:
//     Renderer::getInstance().getCamera().rotate(-0.1,0,0);
//     break;
//   case VK_OEM_PERIOD:
//     Renderer::getInstance().getCamera().rotate(0.1,0,0);
//     break;
  case VK_ADD:
    Renderer::getInstance().getCamera().move(0,0,0.999);
    //Renderer::getInstance().getCamera().zoom(0.001);
    break;
  case VK_SUBTRACT:
    Renderer::getInstance().getCamera().move(0,0,1.001);
    //Renderer::getInstance().getCamera().zoom(-0.001);
    break;
  case 'H':
    Renderer::getInstance().getCamera().zoom(1);
    break;
  case 'J':
    Renderer::getInstance().getCamera().zoom(-1);
    break;
  case 'V':
    toggleVSync();
    break;
  case 'M':
    //Renderer::getInstance().getCamera().toggleGlobalView();
    //void Ship::scrollGravityRef()
    if (shiftPressed_) {
      drawBoundBoxes_ = !drawBoundBoxes_;
    } else {
      relativeOrbits_ = !relativeOrbits_;
    }
    break;
  case 'P':
    if (shiftPressed_) {
      printASM_ = !printASM_;
    } else {
      pause();
    }
    break;
  case 'Y':
    if (shiftPressed_) {
      speed_ += 100;
    } else {
      speedUp();
    }
    break;
  case 'U':
    if (shiftPressed_) {
      speed_ -= 100;
    } else {
      speedDown();
    }
    break;
  case 'X':
    if (shiftPressed_) {
      calcStepLength_ += 2;
    } else {
      calcStepLength_ += 0.05;
    }
    if (calcStepLength_ < 0.0001) {
      calcStepLength_ = 0.05;
    }
    break;
  case 'C':
    if (shiftPressed_) {
      calcStepLength_ -= 2;
    } else {
      calcStepLength_ -= 0.05;
    }
    if (calcStepLength_ < 0.0001) {
      calcStepLength_ = 0.05;
    }
    break;
  case 'O':
    world_->clearTracks();
    break;
  case 0x31:
    toggleTracers();
    break;
  case 0x32:
    auxPrint_ = !auxPrint_;
    break;
  case 0x33:
    toggleGUI();
    break;
  case 0x34:
    //Renderer::getInstance().getCamera().setPitch(0);
    //Renderer::getInstance().getCamera().setHeading(90);
    if (shiftPressed_) {
      world_->getCurrentSystem()->skipTime(1e8);
    } else {
      world_->getCurrentSystem()->skipTime(1e6);
    }
    break;
  case 0x35:
    toggleNames();
    break;
  case 0x36:
    skybox1_ = !skybox1_;
    break;
  case 0x37:
    rotateCameraWithObject_ = !rotateCameraWithObject_;
    //Dirty hack. Temporary. Yeah, I know, it will probably stay here forever...
    if (rotateCameraWithObject_) {
      if (world_->getFollowedObject()) {
        Camera& camera = Renderer::getInstance().getCamera();
        camera.setHeading(camera.getTrueHeading() + world_->getFollowedObject()->getYaw());
      }
    } else {
      if (world_->getFollowedObject()) {
        Camera& camera = Renderer::getInstance().getCamera();
        camera.setHeading(camera.getHeading() - world_->getFollowedObject()->getYaw());
      }
    }
    break;
  case 0xdd:
    break;
    //switchDrawingMode();
    {
      Ship* ship[6];
      ship[0] = static_cast<Ship*>(world_->getObject("Galactica"));
      ship[1] = static_cast<Ship*>(world_->getObject("Ranger"));
      ship[2] = static_cast<Ship*>(world_->getObject("Avenger"));
      ship[3] = static_cast<Ship*>(world_->getObject("Lightning"));
      ship[4] = static_cast<Ship*>(world_->getObject("Thunderbolt"));
      ship[5] = static_cast<Ship*>(world_->getObject("Hawkeye"));
      static int shipNum = -1;
      if (shiftPressed_) {
        --shipNum;
      } else {
        ++shipNum;
      }
      if (shipNum > 5) {
        shipNum = 0;
      }
      if (shipNum < 0) {
        shipNum = 5;
      }
      world_->switchControlledShip(ship[shipNum]);
    //  setShaders("Shaders/TexItems.vert", "Shaders/TexItems.frag", &shader_);
    //  aaa = false;
    //} else {
    //  deleteShaders(shader_);
    //  shader_ = 0;
    //  aaa = true;
    //}
    break;
    }
  case 'B':
    nextState_ = MenuState;

    break;
  default:
    world_->handlePressedKey(key);
    break;
  }
}

bool Gamani::isPressed(int key)
{
  return pressedKeys_.count(key) > 0;
}

void Gamani::handlePressedKeys()
{
  set<int>::iterator itr = pressedKeys_.begin();
  for (; itr != pressedKeys_.end(); ++itr) {
    int key = *itr;
    if (nonContKeys_.count(key) > 0 || layoutManager_.focusGrabbed()) {
      continue;
    }
    //cout << key << " -- " << 0x30 << endl;
    handlePressedKey(key);
  }
}

void Gamani::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  //if (!mouseControl_ && layoutManager_.handleMessage(message, wParam, lParam)) {
  //  return;
  //}
  switch (message) {
  case WM_LBUTTONDOWN:
    layoutManager_.handleMessage(message, wParam, lParam);
    break;
  case WM_RBUTTONDOWN:
    lmDown_ = true;
    lmCoord_[0] = GET_X_LPARAM(lParam);
    lmCoord_[1] = GET_Y_LPARAM(lParam);
    break;
  case WM_MOUSEMOVE:
    layoutManager_.handleMessage(message, wParam, lParam);
    if (!lmDown_ && !rmDown_) {
      break;
    }
    {
      int x = GET_X_LPARAM(lParam);
      int y = GET_Y_LPARAM(lParam);
      int distX = x - lmCoord_[0];
      int distY = y - lmCoord_[1];
      //cout << x << " -- " << lmCoord_[0] << " , " << y << " -- " << lmCoord_[1] << endl;
      if (abs(distX) > 0 || abs(distY) > 0) {
        lmCoord_[0] = x;
        lmCoord_[1] = y;
        lmDrag_ = true;
        Renderer::getInstance().getCamera().rotate(distX/2.0, distY/2.0, 0);
      }
    }
    //if (mouseControl_) {
    //  Renderer::getInstance().getCamera().handleMessage(message, wParam, lParam);
    //}
    break;
  case WM_MOUSEWHEEL:
    {
      int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
      Renderer::getInstance().getCamera().zoom(zDelta / 10.0);
    }
    break;
  case WM_LBUTTONUP:
    layoutManager_.handleMessage(message, wParam, lParam);
    break;
  case WM_RBUTTONUP:
    lmDown_ = false;
    if (0 && !lmDrag_ && interface_) {
      layoutManager_.handleMessage(message, wParam, lParam);
    }
    lmDrag_ = false;
    break;
  case WM_KEYDOWN:
    if (wParam == 0x10) {
      shiftPressed_ = true;
      break;
    }
    pressedKeys_.insert(wParam);
    break;
  case WM_KEYUP:
    if (wParam == 0x10) {
      shiftPressed_ = false;
      break;
    }
    if (pressedKeys_.count(wParam) != 1) {
      break;
    }
    if (nonContKeys_.count(wParam) > 0 || layoutManager_.focusGrabbed()) {
      handlePressedKey(wParam);
    }
    pressedKeys_.erase(wParam);
    break;
  case WM_SIZE:
    Renderer::getInstance().resize(LOWORD(lParam), HIWORD(lParam));
    break;
  default:
    break;
  }
}

void Gamani::switchDrawingMode()
{
  switch (drawingMode_) {
  case GL_LINE_LOOP:
    drawingMode_ = GL_LINE_STRIP;
    break;
  case GL_LINE_STRIP:
    drawingMode_ = GL_TRIANGLES;
    break;
  case GL_TRIANGLES:
    drawingMode_ = GL_TRIANGLE_STRIP;
    break;
  case GL_TRIANGLE_STRIP:
    drawingMode_ = GL_TRIANGLE_FAN;
    break;
  case GL_TRIANGLE_FAN:
    drawingMode_ = GL_QUADS;
    break;
  case GL_QUADS:
    drawingMode_ = GL_QUAD_STRIP;
    break;
  case GL_QUAD_STRIP:
    drawingMode_ = GL_POLYGON;
    break;
  case GL_POLYGON:
    drawingMode_ = GL_LINE_LOOP;
    break;
  default:
    assert(0);
  }

}

void Gamani::speedUp()
{
  if (shiftPressed_) {
    speed_ = 3000;
  } else {
    speed_ *= 2;
  }
}

void Gamani::speedDown()
{
  if (shiftPressed_) {
    speed_ = 1;
  } else {
    speed_ /= 2;
  }
}

void Gamani::pause()
{
  paused_ = !paused_;
}

bool Gamani::run()
{
  while (1) {
    switch (nextState_) {
    case MenuState:
      startMenu();
      endMenu();
      break;
    case GameState:
      startGame();
      endGame();
      break;
    case QuitState:
      return true;
    }
  }
  //bool res = startMenu();
  return true;
}

int body(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, LPSTR& lpCmdLine, int& nShowCmd)
{
  Gamani& inst = Gamani::getInstance();
  bool res = inst.init(hInstance);
  if (!res) {
    return 1;
  }
  res = inst.run();
  if (!res) {
    return 1;
  }
  return 0;
}

int char2bin(char* str, int start, int end)
{
  int res = 0;
  for (int i=start; i<end; ++i) {
    char c = str[i];
    if (c == '0') {
    } else if (c == '1') {
      res++;
    }
    if (i != end-1) {
      res <<= 1;
    }
  }
  return res;
}

double log2(double a)
{
  return log(a)/log(2.0);
}

char* tobinaryfrac(double number)
{
  char* res = new char[53];
  double curr = number;
  for (int i=0; i<52; ++i) {
    curr *= 2.0;
    if (curr >= 1) {
      res[i] = '1';
      curr -= 1.0;
    } else {
      res[i] = '0';
    }
  }
  res[52] = 0;
  return res;
}

double getDouble(double n)
{
  //sign bit, 11-bit exponent, 52-bit mantissa

  //N=M*n^p
  //M - mantiss
  //N = m*2^e
  //log_a(b) = log_e(b)/log_e(a);
  int sign = (n>0)?0:1;
  n = fabs(n);
  double n1 = log2(n);
  double n2 = log2(n/2);

  if (n == 0) {
    n1 = -128;
    n2 = -128;
  }

  double mn = max(fabs(n1), fabs(n2));
  double fn = floor(mn);

  int sign1 = (n1>=0)?1:-1;
  int sign2 = (n2>=0)?1:-1;
  if (sign1 != sign2) {
    sign1 = sign2 = 1;
    fn = 0;
  }

  fn *= sign1;
  int e = fn;
  double p = pow(2.0, e);
  double m = n/p;

  double check = m * p;

  e += 1023;
  if (n == 0) {
    m = 1;
    e = 0;
    sign = 0;
  } else {
    assert (m >= 1 && m < 2);
  }
  m -= 1.0;

  char* mantis = tobinaryfrac(m);
  cout << mantis << endl;
  int exp = e;
  int mant1 = char2bin(mantis, 0, 20);
  int mant2 = char2bin(mantis, 20, 52);
  delete[] mantis;
  int a1 = sizeof(float);
  int a2 = sizeof(double);
  double res;
  
  int* iarr = (int*)&res;
  int i1 = (((sign<<11) + exp)<<20)+mant1;
  int i2 = mant2;
  iarr[0] = i2;
  iarr[1] = i1;
  return res;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Logger::getInstance().log(INFO_LOG_NAME, "Application started");
#ifdef DEBUG
  AllocConsole();
  FILE* stream = NULL;
  errno_t err = freopen_s(&stream, "CON", "w", stdout);
  Logger::getInstance().log(INFO_LOG_NAME, "Console allocated");
  _CrtMemState s1;
  _CrtMemCheckpoint(&s1);
#endif

  ADPCompiler compiler;
  char memory[32000];
  compiler.compile("res/asm/killrot.asm", memory, 32000);
  int lastAddr = compiler.getLastAddr();
  ofstream dumpFile("dump1.txt");
  for (int i=0; i<lastAddr / 8; ++i) {
    if (i%8 == 0) {
      //dumpFile << endl;
    }
    for (int j=0; j<8; ++j) {
      if (j%8 == 0) {
        //dumpFile << " ";
      }
      char bit = getBit(memory[i], j);
      //dumpFile << (char)(bit + '0');
    }
    dumpFile << memory[i];
  }
  dumpFile.close();

  int offset = 0;
  int testAddr = 0;
  vector<Instruction*> instrs;
  while(1) {
    Instruction* instr = ADPFactory::getInstance().createInstr(memory, offset);
    if (!instr) {
      break;
    }
    cout << hex << testAddr/8 << " " << instr->toString() << endl;
    instrs.push_back(instr);
    testAddr += instr->getSize();
    assert (testAddr == offset);
    if (offset >= lastAddr) {
      break;
    }
  }

  //char memory1[32000];
  //int rs[8];
  //for (int i=0; i<8; ++i) {
  //  rs[i] = i;
  //}
  //double fs[8];
  //int psw;
  //auto itr = instrs.begin();
  //Instruction* instr1 = *(itr++);
  //Instruction* instr2 = *(itr++);
  //Instruction* instr3 = *(itr++);
  //Instruction* instr4 = *(itr++);
  //instr1->emit(memory1, rs, fs, psw);
  //instr2->emit(memory1, rs, fs, psw);
  //instr3->emit(memory1, rs, fs, psw);
  //instr4->emit(memory1, rs, fs, psw);
  //int aaa = 0;


  int res = body(hInstance, hPrevInstance, lpCmdLine, nShowCmd);

#ifdef DEBUG
  FreeConsole();
  Logger::getInstance().log(INFO_LOG_NAME, "Console deallocated");
  _CrtMemState s2;
  _CrtMemState s3;
  _CrtMemCheckpoint( &s2 );

  if (_CrtMemDifference(&s3, &s1, &s2)) {
    _CrtMemDumpStatistics( &s3 );
    //_CrtDumpMemoryLeaks();
  }
#endif
  Logger::getInstance().log(INFO_LOG_NAME, CString("Application ended with return code ") + CString(res));
  return res;
}

void Gamani::layoutTest()
{

}

bool Gamani::testInit()
{
  SystemParser parser;
  StarSystem* parsedSystem = parser.parseSystem(fileToload_, &layoutManager_);
  if (!parsedSystem) {
    nextState_ = MenuState;
    return false;
    //exit(1);
  }
  world_->setStarSystem(parsedSystem);
  Ship* playerShip = parser.getPlayerShip();
  const vector<AstralBody*>& freeObjects = parser.getFreeObjects();
  for (uint32_t i=0; i<freeObjects.size(); ++i) {
    world_->addFreeObject(freeObjects[i]);
  }
  world_->switchControlledShip(playerShip);
  //Star* star = *parsedSystem->getStars().begin();
  //Renderer::getInstance().getParticleManager()->addParticle(ParticleManager::StarParticle, star, 0, 0, -1, 0.00001);
  return true;
  //Renderer::getInstance().formatDistance(1002342354.234234);
  /*
  Distance 1e6 km = 1e9 m   ;;; 1e6 m = 1e3 km
  Mass     1e6 kg
  Speed    1 km/s = 1000m/s ;;; 1 m/s
  Time     1 pass = speedReduce sec
  Earth:
  Distance 148.6e6 km
  Velocity 30 km/s
  Mass 6000e8 t = 6000e11 kg = 6000e5 e6 kg
  Sun:
  Mass 330000*Earth = (6000*330000e11) e6 kg = 1980000000e11 e6 kg = 198e18 e6 kg
  */

  //Sun
  //Star* star = new Star();
  //star->setMass(1.98e24);
  //star->setRadius(695.5);
  //star->setName("Sun");

  //Planet* planet = NULL;
  //Planet* satellite = NULL;

  ////Mercury
  //planet = new Planet();
  //planet->setCoord(Vector3(57900, 0, 0));
  //planet->setRadius(2.4397);
  //planet->setMass(3.3e17);
  //planet->setVelocity(Vector3(0, -47870, 0));
  //planet->setName("Mercury");
  //planet->setRotationPeriod(1407.5*3600);
  //star->addSatellite(planet);
  //
  ////Venus
  //planet = new Planet();
  //planet->setCoord(Vector3(108000, 0, 0));
  //planet->setRadius(6.0518);
  //planet->setMass(4.8685e18);
  //planet->setVelocity(Vector3(0, -35000, 0));
  //planet->setName("Venus");
  //planet->setRotationPeriod(243.0185*24*3600);
  //star->addSatellite(planet);

  ////Earth
  //planet = new Planet();
  //planet->setCoord(Vector3(149600, 0, 0));
  //planet->setRadius(6.371);
  //planet->setMass(6.0e18);
  //planet->setVelocity(Vector3(0, -29783, 0));
  //planet->setName("Earth");
  //planet->setRotationPeriod(24*3600);
  //planet->setAtmRadius(6.971);
  //planet->setAtmColor(Vector3(0, 0.75, 1));
  //star->addSatellite(planet);

  //Ship* ship = new Ship();
  //ship->setCoord(Vector3(149600 - /*6.371*/10, 0, 0));
  ////ship->setCoord(Vector3(149599, -374.399, 0));
  //ship->setVelocity(Vector3(0/*-6250*/, -29783-6250, 0));
  ////ship->setVelocity(Vector3(-1022, 29783, 0));
  //ship->setRadius(0.00005);
  ////ship->setGravityRef(planet);
  //ship->setName("Galactica");
  //HUD* shipHud = new HUD();
  //shipHud->AddLayout.connect(&layoutManager_, &LayoutManager::addLayout);
  //shipHud->RemoveLayout.connect(&layoutManager_, &LayoutManager::removeLayout);
  //shipHud->init(ship);
  ////ship->setHUD(shipHud);
  //ship->test1_ = planet;
  //ship->initModel("res/ss2.3ds");
  ////ship->setDockingPort(Vector3(0, 1.3, 0)); //Hey, screwed up axes. Again...
  //ship->setPortAngle(0);

  //Station* station = new Station();
  //station->setCoord(Vector3(149590.01, 0, 0));
  //station->setVelocity(Vector3(/*-6250*/0, -29783-6250, 0));
  //station->setRadius(0.001);
  //station->setName("Shipyard");
  ////StationDisplay* stationDisplay = new StationDisplay();
  ////stationDisplay->init();
  ////stationDisplay->setVisible(false);
  ////layoutManager_.addLayout(stationDisplay);
  ////station->setDisplay(stationDisplay);
  //station->initModel("res/station.3ds");
  ////station->setDockingPort(Vector3(0.65, 0.03, 0.1));
  //station->setPortAngle(270);
  //planet->addSatellite(station);

  ////station->setYawVel(1);
  ////ship->dockedTo_ = station;
  ////ship->docked_ = true;


  ////Moon
  //satellite = new Planet();
  //satellite->setCoord(Vector3(149600, -374.399, 0));
  //satellite->setRadius(1.736);
  //satellite->setMass(7.3477e16);
  //satellite->setVelocity(Vector3(-1022, -29783, 0));
  ////satellite->setVelocity(Vector3(0, 29783, 0));
  //satellite->setColor(Vector3(1,1,1));
  //satellite->setName("Moon");
  //satellite->setRotationPeriod(27.321582*24*3600);
  //planet->addSatellite(satellite);
  //ship->test2_ = satellite;

  ////Mars
  //planet = new Planet();
  //planet->setCoord(Vector3(249600, 0, 0));
  //planet->setRadius(3.386);
  //planet->setMass(6.4185e17);
  //planet->setVelocity(Vector3(0, -24130, 0));
  //planet->setName("Mars");
  //planet->setRotationPeriod(88776);
  //planet->setAtmColor(Vector3(216.0/255.0, 134.0/255.0, 79.0/255.0));
  //planet->setAtmRadius(3.496);
  //star->addSatellite(planet);

  ////Phobos
  //satellite = new Planet();
  //satellite->setCoord(Vector3(249600, 9.380, 0));
  //satellite->setRadius(11.1e-3);
  //satellite->setMass(1.972e10);
  //satellite->setVelocity(Vector3(1840, -24130, 0));
  //satellite->setName("Phobos");
  //satellite->setRotationPeriod(7*3600 + 39.2*60);
  //planet->addSatellite(satellite);

  ////Deimos
  //satellite = new Planet();
  //satellite->setCoord(Vector3(249600, 23.460, 0));
  //satellite->setRadius(6.2e-3);
  //satellite->setMass(1.48e9);
  //satellite->setVelocity(Vector3(1350, -24130, 0));
  //satellite->setName("Deimos");
  //satellite->setRotationPeriod(1.26244 * 24 * 3600);
  //planet->addSatellite(satellite);

  ////Jupiter
  //planet = new Planet();
  //planet->setCoord(Vector3(760000, 0, 0));
  //planet->setRadius(71.492);
  //planet->setMass(1.8986e21);
  //planet->setVelocity(Vector3(0, -13070, 0));
  //planet->setName("Jupiter");
  //planet->setRotationPeriod(9.925*3600);
  //star->addSatellite(planet);

  ////Io
  //satellite = new Planet();
  //satellite->setCoord(Vector3(760000, 421.7, 0));
  //satellite->setRadius(1.321);
  //satellite->setMass(8.9319e16);
  //satellite->setVelocity(Vector3(17430, -13070, 0));
  //satellite->setColor(Vector3(1,1,1));
  //satellite->setName("Io");
  //satellite->setRotationPeriod(1.769137786*24*3600);
  //planet->addSatellite(satellite);

  ////Europa
  //satellite = new Planet();
  //satellite->setCoord(Vector3(760000, 670.99, 0));
  //satellite->setRadius(1.569);
  //satellite->setMass(4.8e16);
  //satellite->setVelocity(Vector3(13740, -13070, 0));
  //satellite->setColor(Vector3(1,1,1));
  //satellite->setName("Europa");
  //satellite->setRotationPeriod(3.551181*24*3600);
  //planet->addSatellite(satellite);

  ////Ganymede
  //satellite = new Planet();
  //satellite->setCoord(Vector3(760000, 1070.4, 0));
  //satellite->setRadius(2.6341);
  //satellite->setMass(1.4819e17);
  //satellite->setVelocity(Vector3(10880, -13070, 0));
  //satellite->setColor(Vector3(1,1,1));
  //satellite->setName("Ganymede");
  //satellite->setRotationPeriod(7.15455296*24*3600);
  //planet->addSatellite(satellite);

  ////Callisto
  //satellite = new Planet();
  //satellite->setCoord(Vector3(760000, 1882.7, 0));
  //satellite->setRadius(2.4103);
  //satellite->setMass(1.0759e17);
  //satellite->setVelocity(Vector3(8210, -13070, 0));
  //satellite->setColor(Vector3(1,1,1));
  //satellite->setName("Callisto");
  //satellite->setRotationPeriod(16.6890184*24*3600);
  //planet->addSatellite(satellite);

  ////Saturn
  //planet = new Planet();
  //planet->setCoord(Vector3(1420000, 0, 0));
  //planet->setRadius(60.268);
  //planet->setMass(5.6846e20);
  //planet->setVelocity(Vector3(0, -9690, 0));
  //planet->setName("Saturn");
  //planet->setRotationPeriod(10*3600 + 34*60 + 13);
  //star->addSatellite(planet);

  ////Uranus
  //planet = new Planet();
  //planet->setCoord(Vector3(2876679.082, 0, 0));
  //planet->setRadius(25.559);
  //planet->setMass(8.6832e19);
  //planet->setVelocity(Vector3(0, -6810, 0));
  //planet->setName("Uranus");
  //planet->setRotationPeriod(0.71833 *24*3600);
  //star->addSatellite(planet);

  ////Neptune
  //planet = new Planet();
  //planet->setCoord(Vector3(4503443.661, 0, 0));
  //planet->setRadius(24.341);
  //planet->setMass(1.0243e20);
  //planet->setVelocity(Vector3(0, -5430, 0));
  //planet->setName("Neptune");
  //planet->setRotationPeriod(0.6713 *24*3600);
  //star->addSatellite(planet);

  //StarSystem* system = new StarSystem();
  //system->addStar(star);

  ////system->skipTime(1e15);

  //world_->setStarSystem(system);

  ////world_->addFreeObject(station);
  //world_->addFreeObject(ship);
  //world_->switchControlledShip(ship);
}


