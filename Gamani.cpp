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

#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif //DEBUG
#include "Station.h"
#include "StationDisplay.h"

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

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

Gamani::Gamani():world_(new World()), paused_(true), speed_(1.0), calcStepLength_(0.05), dtModifier_(50),auxAxes_(false),lmDown_(false),rmDown_(false),
  lmDrag_(false), rmDrag_(false), tracers_(false), auxPrint_(true), interface_(true), names_(false)
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
  nonContKeys_.insert('Q');
  nonContKeys_.insert('E');
  nonContKeys_.insert('O');
  nonContKeys_.insert('R');
  nonContKeys_.insert('K');
  nonContKeys_.insert(0x30);
  nonContKeys_.insert(0x31);
  nonContKeys_.insert(0x32);
  nonContKeys_.insert(0x33);
  nonContKeys_.insert(0x34);
  nonContKeys_.insert(0x35);
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
  testInit();
  Renderer& renderer = Renderer::getInstance();
  renderer.init(hIhstance);
  renderer.changeRenderList(world_->getAllObjects());
  layoutManager_.setRenderer(new GUIRenderer());
  return true;
}

bool Gamani::mainLoop()
{
  setVSync(0);
  int frame = 0;
  long timebase=GetTickCount();
  long currentTime = GetTickCount();
  long time;

  double accumulator = 0.0f;
  double dt = 1.0f;

  double accumKeys = 0.0f;
  double dtKeys = 1;

  double traceT = 0.0f;
  double traceDT = 500.0f;

  /************************************************************************/
  /* Interface test initialization. Temporary code                        */
  /************************************************************************/
  //NavDisplay* form = new NavDisplay();
  //form->init();
  //layoutManager_.addLayout(form);
  /************************************************************************/
  /* End of interface test initialization                                 */
  /************************************************************************/

  seconds_ = 0;

  world_->selectShip();

  upperPanel_ = new UpperPanel();
  upperPanel_->init();
  layoutManager_.addLayout(upperPanel_);

  MSG msg={0};
  while(WM_QUIT!=msg.message) {
    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      time = GetTickCount();
      ++frame;
      double delta = time - currentTime;
      currentTime = time;
      accumulator += delta;
      accumKeys += delta;
      traceT += delta;

      if (speed_ < 1) {
        dt = 1.0f/speed_;
      } else {
        dt = 1.0f;
      }

      dt *= dtModifier_;

      while (traceT >= traceDT) {
        if (!paused_) {
          world_->snapshot();
        }
        traceT -= traceDT;
      }

      while(accumulator >= dt) {
        if (!paused_) {
          for (int i=0; i<speed_; ++i) {
            seconds_ += calcStepLength_*dt * (100/dtModifier_);
            world_->interactionStep();
          }
        }
        accumulator -= dt;
      }

      while (accumKeys >= dt) {
        for (int i=0; i<speed_; ++i) {
          handlePressedKeys();
        }
        accumKeys -= dt;
      }

      if (time - timebase > 1000) {
        double fps = frame*1000.0/(time-timebase);
        timebase = time;
        frame = 0;
        char cfps[200];
        sprintf(cfps, "FPS: %.lf IPS: %.lf", fps, speed_*1000/dt);
        SetWindowTextA(Renderer::getInstance().getHwnd(), (LPCSTR)(cfps));
        if (fps < 5) {
          paused_ = true;
        }
      }

      Renderer::getInstance().render();
      if (interface_) {
        layoutManager_.render();
      }
      Renderer::getInstance().renderEnd();

      GLenum err = glGetError();

      if (err != 0) {
        return false;
      }
    }
  }

  return true;

}

void Gamani::handlePressedKey(int key)
{
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
    Renderer::getInstance().getCamera().toggleGlobalView();
    break;
  case 'P':
    pause();
    //paused_ = !paused_;
    break;
  case 'Y':
    speedUp();
    //speed_ *= 2;
    break;
  case 'U':
    speedDown();
    //speed_ /= 2;
    break;
  case 'X':
    calcStepLength_ *= 1.5;
    break;
  case 'C':
    calcStepLength_ /= 1.5;
    break;
  case 'O':
    auxAxes_ = !auxAxes_;
    break;
  case 0x31:
    tracers_ = !tracers_;
    break;
  case 0x32:
    auxPrint_ = !auxPrint_;
    break;
  case 0x33:
    interface_ = !interface_;
    break;
  case 0x34:
    Renderer::getInstance().getCamera().setPitch(0);
    Renderer::getInstance().getCamera().setHeading(90);
    break;
  case 0x35:
    names_ = !names_;
    break;
  default:
    world_->handlePressedKey(key);
    break;
  }
}

void Gamani::handlePressedKeys()
{
  set<int>::iterator itr = pressedKeys_.begin();
  for (; itr != pressedKeys_.end(); ++itr) {
    int key = *itr;
    if (nonContKeys_.count(key) > 0) {
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
    lmDown_ = true;
    lmCoord_[0] = GET_X_LPARAM(lParam);
    lmCoord_[1] = GET_Y_LPARAM(lParam);
    break;
  case WM_MOUSEMOVE:
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
    lmDown_ = false;
    if (!lmDrag_) {
      layoutManager_.handleMessage(message, wParam, lParam);
    }
    lmDrag_ = false;
    break;
  case WM_KEYDOWN:
    pressedKeys_.insert(wParam);
    break;
  case WM_KEYUP:
    assert(pressedKeys_.count(wParam) == 1);
    if (nonContKeys_.count(wParam) > 0) {
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

void Gamani::speedUp()
{
  speed_ *= 2;
}

void Gamani::speedDown()
{
  speed_ /= 2;
}

void Gamani::pause()
{
  paused_ = !paused_;
}

bool Gamani::run()
{
  bool res = mainLoop();
  return res;
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef DEBUG
  AllocConsole();
  FILE* stream = NULL;
  errno_t err = freopen_s(&stream, "CON", "w", stdout);
  _CrtMemState s1;
  _CrtMemCheckpoint( &s1 );
#endif

  int res = body(hInstance, hPrevInstance, lpCmdLine, nShowCmd);

#ifdef DEBUG
  FreeConsole();
  _CrtMemState s2;
  _CrtMemState s3;
  _CrtMemCheckpoint( &s2 );

  if ( _CrtMemDifference( &s3, &s1, &s2) )
    _CrtMemDumpStatistics( &s3 );
#endif
  return res;
}

void Gamani::layoutTest()
{

}

void Gamani::testInit()
{
  Renderer::getInstance().formatDistance(1002342354.234234);
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
  Star* star = new Star();
  star->setMass(1.98e24);
  star->setRadius(695.5);
  star->setName("Sun");

  Planet* planet = NULL;
  Planet* satellite = NULL;

  //Mercury
  planet = new Planet();
  planet->setCoord(Vector3(57900, 0, 0));
  planet->setRadius(2.4397);
  planet->setMass(3.3e17);
  planet->setVelocity(Vector3(0, 47870, 0));
  planet->setName("Mercury");
  planet->setRotationPeriod(1407.5*3600);
  star->addSatellite(planet);
  
  //Venus
  planet = new Planet();
  planet->setCoord(Vector3(108000, 0, 0));
  planet->setRadius(6.0518);
  planet->setMass(4.8685e18);
  planet->setVelocity(Vector3(0, 35000, 0));
  planet->setName("Venus");
  planet->setRotationPeriod(-243.0185*24*3600);
  star->addSatellite(planet);

  //Earth
  planet = new Planet();
  planet->setCoord(Vector3(149600, 0, 0));
  planet->setRadius(6.371);
  planet->setMass(6.0e18);
  planet->setVelocity(Vector3(0, 29783, 0));
  planet->setName("Earth");
  planet->setRotationPeriod(24*3600);
  planet->setAtmRadius(6.471);
  planet->setAtmColor(Vector3(0, 0.75, 1));
  star->addSatellite(planet);

  Ship* ship = new Ship();
  ship->setCoord(Vector3(149590, 0, 0));
  ship->setVelocity(Vector3(/*-6250*/1, 29783-6250, 0));
  ship->setRadius(0.001);
  ship->setGravityRef(planet);
  ship->setName("Galactica");
  HUD* shipHud = new HUD();
  shipHud->AddLayout.connect(&layoutManager_, &LayoutManager::addLayout);
  shipHud->RemoveLayout.connect(&layoutManager_, &LayoutManager::removeLayout);
  shipHud->init();
  ship->setHUD(shipHud);
  ship->test1_ = planet;
  ship->initModel("res/new.3ds");
  ship->setDockingPort(Vector3(0,0.25,1.4)); //Hey, screwed up axes. Again...
  ship->setPortAngle(0);

  Station* station = new Station();
  station->setCoord(Vector3(149590.1, 0, 0));
  station->setVelocity(Vector3(/*-6250*/0, 29783-6250, 0));
  station->setRadius(0.01);
  station->setName("Shipyard");
  //StationDisplay* stationDisplay = new StationDisplay();
  //stationDisplay->init();
  //layoutManager_.addLayout(stationDisplay);
  station->initModel("res/station.3ds");
  station->setDockingPort(Vector3(0.6,1.1,0.05));
  station->setPortAngle(270);


  //Moon
  satellite = new Planet();
  satellite->setCoord(Vector3(149600, -374.399, 0));
  satellite->setRadius(1.736);
  satellite->setMass(7.3477e17);
  satellite->setVelocity(Vector3(-1022, 29783, 0));
  satellite->setColor(Vector3(1,1,1));
  satellite->setName("Moon");
  satellite->setRotationPeriod(27.321582*24*3600);
  planet->addSatellite(satellite);
  ship->test2_ = satellite;

  //Mars
  planet = new Planet();
  planet->setCoord(Vector3(249600, 0, 0));
  planet->setRadius(3.386);
  planet->setMass(6.4185e17);
  planet->setVelocity(Vector3(0, 24130, 0));
  planet->setName("Mars");
  planet->setRotationPeriod(88776);
  star->addSatellite(planet);

  //Phobos
  satellite = new Planet();
  satellite->setCoord(Vector3(249600, 9.380, 0));
  satellite->setRadius(11.1e-3);
  satellite->setMass(1.972e10);
  satellite->setVelocity(Vector3(1840, 24130, 0));
  satellite->setName("Phobos");
  satellite->setRotationPeriod(7*3600 + 39.2*60);
  planet->addSatellite(satellite);

  //Deimos
  satellite = new Planet();
  satellite->setCoord(Vector3(249600, 23.460, 0));
  satellite->setRadius(6.2e-3);
  satellite->setMass(1.48e9);
  satellite->setVelocity(Vector3(1350, 24130, 0));
  satellite->setName("Deimos");
  satellite->setRotationPeriod(1.26244 * 24 * 3600);
  planet->addSatellite(satellite);

  //Jupiter
  planet = new Planet();
  planet->setCoord(Vector3(760000, 0, 0));
  planet->setRadius(71.492);
  planet->setMass(1.8986e21);
  planet->setVelocity(Vector3(0, 13070, 0));
  planet->setName("Jupiter");
  planet->setRotationPeriod(9.925*3600);
  star->addSatellite(planet);

  //Io
  satellite = new Planet();
  satellite->setCoord(Vector3(760000, 421.7, 0));
  satellite->setRadius(1.321);
  satellite->setMass(8.9319e16);
  satellite->setVelocity(Vector3(17430, 13070, 0));
  satellite->setColor(Vector3(1,1,1));
  satellite->setName("Io");
  satellite->setRotationPeriod(1.769137786*24*3600);
  planet->addSatellite(satellite);

  //Europa
  satellite = new Planet();
  satellite->setCoord(Vector3(760000, 670.99, 0));
  satellite->setRadius(1.569);
  satellite->setMass(4.8e16);
  satellite->setVelocity(Vector3(13740, 13070, 0));
  satellite->setColor(Vector3(1,1,1));
  satellite->setName("Europa");
  satellite->setRotationPeriod(3.551181*24*3600);
  planet->addSatellite(satellite);

  //Ganymede
  satellite = new Planet();
  satellite->setCoord(Vector3(760000, 1070.4, 0));
  satellite->setRadius(2.6341);
  satellite->setMass(1.4819e17);
  satellite->setVelocity(Vector3(10880, 13070, 0));
  satellite->setColor(Vector3(1,1,1));
  satellite->setName("Ganymede");
  satellite->setRotationPeriod(7.15455296*24*3600);
  planet->addSatellite(satellite);

  //Callisto
  satellite = new Planet();
  satellite->setCoord(Vector3(760000, 1882.7, 0));
  satellite->setRadius(2.4103);
  satellite->setMass(1.0759e17);
  satellite->setVelocity(Vector3(8210, 13070, 0));
  satellite->setColor(Vector3(1,1,1));
  satellite->setName("Callisto");
  satellite->setRotationPeriod(16.6890184*24*3600);
  planet->addSatellite(satellite);

  //Saturn
  planet = new Planet();
  planet->setCoord(Vector3(1420000, 0, 0));
  planet->setRadius(60.268);
  planet->setMass(5.6846e20);
  planet->setVelocity(Vector3(0, 9690, 0));
  planet->setName("Saturn");
  planet->setRotationPeriod(10*3600 + 34*60 + 13);
  star->addSatellite(planet);

  //Uranus
  planet = new Planet();
  planet->setCoord(Vector3(2876679.082, 0, 0));
  planet->setRadius(25.559);
  planet->setMass(8.6832e19);
  planet->setVelocity(Vector3(0, 6810, 0));
  planet->setName("Uranus");
  planet->setRotationPeriod(0.71833 *24*3600);
  star->addSatellite(planet);

  //Neptune
  planet = new Planet();
  planet->setCoord(Vector3(4503443.661, 0, 0));
  planet->setRadius(24.341);
  planet->setMass(1.0243e20);
  planet->setVelocity(Vector3(0, 5430, 0));
  planet->setName("Neptune");
  planet->setRotationPeriod(0.6713 *24*3600);
  star->addSatellite(planet);

  StarSystem* system = new StarSystem();
  system->addStar(star);
  world_->setStarSystem(system);

  world_->addFreeObject(station);
  world_->addFreeObject(ship);
  world_->switchControlledShip(ship);
}


