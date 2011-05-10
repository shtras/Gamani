#pragma once
#include "Widget.h"

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
  void setDimensions(int left, int top, int width, int height);
  int getTop() {return top_;}
  int getLeft() {return left_;}
  int getWidth() {return width_;}
  int getHeight() {return height_;}
  int getRight() {return left_ + width_;}
  int getBottom() {return top_ - height_;}
  bool isInside(int x, int y);
  bool handleMouseClick(int x, int y);
  void addWidgetToGC(Widget* widget) {widgetsToGC_.push_back(widget);}

protected:
  list<Widget*> widgets_;
  list<Widget*> widgetsToGC_;
  int top_;
  int left_;
  int width_;
  int height_;
  bool visible_;
};

