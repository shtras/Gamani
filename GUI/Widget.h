#pragma once

class WLayout;

class Widget
{
public:
  Widget();
  virtual ~Widget();

  virtual void render(double left, double top, double width, double height) = 0;
  virtual void setDimensions(double left, double top, double width, double height);
  double getTop() {return top_;}
  double getLeft() {return left_;}
  double getWidth() {return width_;}
  double getHeight() {return height_;}
  double getRight() {return left_ + width_;}
  double getBottom() {return top_ - height_;}
  bool isInside(double x, double y);
  virtual void setVisible(bool value) {visible_ = value;}
  bool isVisible() {return visible_;}
  virtual void click() = 0;
  virtual bool isInteractive() {return false;}
  virtual void collectGarbage() {}
  virtual bool grabsFocus() {return false;}
  virtual void getFocus() {}
  virtual void loseFocus() {}
  virtual bool handlePressedKey(int key) {assert(0); return false;}
  bool isHovered() {return hovered_;}
  void setHovered(bool val) {hovered_ = val;}
  void setPressed(bool val) {pressed_ = val;}
  bool isPressed() {return pressed_;}
  void setLayout(WLayout* layout) {container_ = layout;}
protected:
  double getActualHeight();
  WLayout* container_;
  double top_;
  double left_;
  double width_;
  double height_;

  bool visible_;
  bool hovered_;
  bool pressed_;
};

