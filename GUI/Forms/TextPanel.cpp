#include "StdAfx.h"
#include "TextPanel.h"

TextPanel::TextPanel():textBox_(NULL)
{

}

TextPanel::~TextPanel()
{

}

void TextPanel::init()
{
  textBox_ = new WTextBox();
  textBox_->setDimensions(0, 0.05, 1, 0.9);
  addWidget(textBox_);

  WButton* closeButton = new WButton();
  closeButton->setDimensions(0.4, 0, 0.2, 0.05);
  closeButton->setLabel("Close");
  closeButton->sigClick.connect(this, &TextPanel::closeButtonClick);
  addWidget(closeButton);
}

void TextPanel::setText(const vector<CString>& text)
{
  textBox_->reset();
  for (auto itr = text.begin(); itr != text.end(); ++itr) {
    CString line = *itr;
    textBox_->addLine(line);
  }
}

void TextPanel::closeButtonClick()
{
  setVisible(false);
}
