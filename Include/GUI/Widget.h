#pragma once

class Widget
{
public:
  Widget();
  virtual ~Widget();

  virtual void render(double left, double top, double width, double height) = 0;
  void setDimensions(int left, int top, int width, int height);
  int getTop() {return top_;}
  int getLeft() {return left_;}
  int getWidth() {return width_;}
  int getHeight() {return height_;}
  int getRight() {return left_ + width_;}
  int getBottom() {return top_ - height_;}
  bool isInside(int x, int y);
  virtual void setVisible(bool value) {visible_ = value;}
  bool isVisible() {return visible_;}
  virtual void click() = 0;
  virtual bool isInteractive() {return false;}
  virtual void collectGarbage() {}
protected:
  int top_;
  int left_;
  int width_;
  int height_;

  bool visible_;
};

