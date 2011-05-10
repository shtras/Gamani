#include "StdAfx.h"
#include "Widget.h"


Widget::Widget():top_(0), left_(0), width_(0), height_(0), visible_(true)
{
}

Widget::~Widget()
{
}

void Widget::setDimensions(int left, int top, int width, int height)
{
  top_ = top;
  left_ = left;
  width_ = width;
  height_ = height;
}

bool Widget::isInside(int x, int y)
{
  return ((x >= left_) && (x <= left_+width_) && (y >= top_-height_) && (y <= top_));
}
