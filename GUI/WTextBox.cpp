#include "StdAfx.h"

#include "WTextBox.h"
#include "Renderer.h"

WTextBox::WTextBox():
topLineIdx_(0), visibleLines_(0)
{

}

WTextBox::~WTextBox()
{

}

void WTextBox::setDimensions(double left, double top, double width, double height)
{
  Widget::setDimensions(left, top, width, height);
}

void WTextBox::render(double left, double top, double width, double height)
{
  double actualHeight = getActualHeight();
  visibleLines_ = actualHeight / LINE_HEIGHT;
  /*
  height: 50
  line: 10
  lines: 5
  height: 0.7
  0.7 / 5 = 0.14 <===
  */
  double delta = height / visibleLines_;
  uint32_t tillLine = min(topLineIdx_ + visibleLines_, lines_.size());
  for (uint32_t i=topLineIdx_; i<tillLine; ++i) {
    CString line = lines_[i];
    Renderer::getInstance().textOut(left + 0.1, top + height - i * delta, -0.1, "%s", line.operator const char *());
  }
}

void WTextBox::addLine(CString line)
{
  lines_.push_back(line);
}

void WTextBox::reset()
{
  lines_.clear();
}
