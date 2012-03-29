#pragma once
#include "Widget.h"
#include "WLayout.h"

class WMenu: public Widget
{
public:
  class Item: public Widget
  {
  public:
    Item();
    ~Item();
    virtual void render(double left, double top, double width, double height);
    virtual void click();
    void setLabel(CString label) {label_ = label;}
    CString getLabel() {return label_;}
    bool isInteractive() {return true;}
    void setValue(void* value) {value_ = value;}
    signal1<void*> sigClick;
  private:
    CString label_;
    void* value_;
  };

  WMenu();
  ~WMenu();
  virtual void render(double left, double top, double width, double height);
  virtual void click() {}
  void addItem(Item* item) {items_.push_back(item);}
  bool isInteractive() {return false;}
  virtual void setVisible(bool value);
  virtual void collectGarbage();

  void initialClear(WLayout* layout);
  void finishClear();
private:
  list<Item*> items_;
};
