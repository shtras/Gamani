#pragma once
#include "Widget.h"
#include "WButton.h"

class WLayout: public has_slots<>
{
public:
  WLayout(int top = 0, int left = 0, int height = 0, int width = 0);
  virtual ~WLayout();
  virtual void init() = 0;
  virtual void render();
  virtual void renderEnd();
  void addWidget(Widget* widget);
  void removeWidget(Widget* widget);
  void setDimensions(double left, double top, double width, double height);
  double getTop() {return top_;}
  double getLeft() {return left_;}
  double getWidth() {return width_;}
  double getHeight() {return height_;}
  double getRight() {return left_ + width_;}
  double getBottom() {return top_ - height_;}
  bool isInside(double x, double y);
  Widget* handleMouseEvent(UINT message, WPARAM wparam, double x, double y);
  void addWidgetToGC(Widget* widget) {widgetsToGC_.push_back(widget);}
  bool isVisible() {return visible_;}
  bool isSquare() {return square_;}
  bool isRightAlign() {return rightAlign_;}
  void setVisible(bool value) {visible_ = value;}
  void setRightAlign(bool value) {rightAlign_ = value;}
  void setBottom(bool value) {bottom_ = value;}
  bool isHovered() {return hovered_;}
  void setHovered(bool val);
  void setMinimizible(bool value);
protected:
  void minimize();
  WButton* minimizeButton_;
  list<Widget*> widgets_;
  list<Widget*> widgetsToGC_;
  double top_;
  double left_;
  double width_;
  double height_;
  double maxTop_;
  double maxLeft_;
  double maxWidth_;
  double maxHeght_;
  bool visible_;
  bool square_;
  bool rightAlign_;
  bool minimized_;
  bool bottom_;
  bool hovered_;
private:
};

