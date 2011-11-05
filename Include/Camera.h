#pragma once
#include <Windows.h>
#include "AstralBody.h"

class Camera
{
public:
  Camera();
  ~Camera();
  void position();
  void applyZoom();
  void test();
  void setAspect(double aspect);
  void move(double x, double y, double z);
  void position(Vector3 pos);
  void position(AstralBody* body);
  void rotate(double x, double y, double z);
  void zoom(double val);
  void toggleGlobalView();
  void reset();
  void handleMessage(UINT message, WPARAM wParam, LPARAM lParam);
  const double* getPosition() const {return position_;}
  double getZoom() const {return zoom_;}
  double getHeading() const;
  double getPitch() const {return pitch_;}
  void setPitch(double pitch) {pitch_ = pitch;}
  void setHeading(double heading) {heading_ = heading;}
private:
  double position_[3];
  double aspect_;
  double heading_;
  double pitch_;
  double zoom_;
  double lastZoom_;
  bool globalView_;
  int lastX_;
  int lastY_;
};
