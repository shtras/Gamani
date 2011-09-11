#include "StdAfx.h"
#include "WMenu.h"
#include "Renderer.h"

WMenu::WMenu()
{

}

WMenu::~WMenu()
{
  //list<Item*>::iterator itr = items_.begin();
  //for (; itr != items_.end(); ++itr) {
  //  Item* item = *itr;
  //  delete item;
  //}
}

void WMenu::render(double left, double top, double width, double height)
{
  //glDisable(GL_LIGHTING);
  //glColor3f(0.2, 0.2, 0.2);
  //glBegin(GL_LINE_LOOP);
  //glVertex3f(0, 0, -0.09);
  //glVertex3f(width, 0, -0.09);
  //glVertex3f(width, height, -0.09);
  //glVertex3f(0, height, -0.09);
  //glEnd();

  //glColor3f(0.8,0.2,0.7);
  //glBegin(GL_POLYGON);
  //glVertex3f(0, 0, -0.09);
  //glVertex3f(width, 0, -0.09);
  //glVertex3f(width, height, -0.09);
  //glVertex3f(0, height, -0.09);
  //glEnd();
  //glEnable(GL_LIGHTING);
}

void WMenu::setVisible(bool value)
{
  list<Item*>::iterator itr = items_.begin();
  for (; itr != items_.end(); ++itr) {
    Item* item = *itr;
    item->setVisible(value);
  }
  Widget::setVisible(value);
}

void WMenu::initialClear(WLayout* layout)
{
  list<Item*>::iterator itr = items_.begin();
  for (; itr != items_.end(); ++itr) {
    Item* item = *itr;
    layout->removeWidget(item);
  }
}

void WMenu::finishClear()
{
  list<Item*>::iterator itr = items_.begin();
  for (; itr != items_.end(); ++itr) {
    Item* item = *itr;
    item->sigClick.disconnect_all();
    delete item;
  }
  items_.clear();
}

void WMenu::collectGarbage()
{
  finishClear();
}

WMenu::Item::Item()
{

}

WMenu::Item::~Item()
{

}

void WMenu::Item::render(double left, double top, double width, double height)
{
  glDisable(GL_LIGHTING);
  glColor3f(0.3, 0.4, 0.6);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, -0.1);
  glVertex3f(width, 0, -0.1);
  glVertex3f(width, height, -0.1);
  glVertex3f(0, height, -0.1);
  glEnd();


  glColor4f(0.12,0.15,0.35, 0.9);
  glBegin(GL_POLYGON);
  glVertex3f(0, 0, -0.1);
  glVertex3f(width, 0, -0.1);
  glColor4f(0.7,0.21,0.65, 0.9);
  glVertex3f(width, height, -0.1);
  glVertex3f(0, height, -0.1);
  glEnd();

  glColor3f(0.8,0.8,0.8);
  Renderer::getInstance().textOutNoMove(0.01, height/2, -0.11, "%s", label_.operator const char *());
  glEnable(GL_LIGHTING);
}

void WMenu::Item::click()
{
  sigClick.emit((void*)value_);
}
