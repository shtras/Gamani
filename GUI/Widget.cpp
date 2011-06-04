#include "StdAfx.h"
#include "Widget.h"


Widget::Widget():top_(0), left_(0), width_(0), height_(0), visible_(true)
{
}

Widget::~Widget()
{
}

void Widget::setDimensions(double left, double top, double width, double height)
{
  top_ = top;
  left_ = left;
  width_ = width;
  height_ = height;
}

bool Widget::isInside(double x, double y)
{
  return ((x >= left_) && (x <= left_+width_) && (y >= top_) && (y <= top_ + height_));
}
