#ifndef BASE_ITEM_ZERO_WIDGET_H
#define BASE_ITEM_ZERO_WIDGET_H

#include "LcdMenu.h"
#include "MenuItem.h"
#include <utils/utils.h>

class BaseItemZeroWidget : public MenuItem {
  public:
    explicit BaseItemZeroWidget(const char* text)
        : MenuItem(text) {}

  protected:
    virtual void handleCommit() = 0;
    void draw(DisplayInterface* display, const uint8_t row) override {
        const size_t len = strlen(this->text);
        display->drawAt(row, 1, const_cast<char*>(this->text));
        display->clearAfter(row, 1 + len);
    }
    bool process(LcdMenu* menu, const unsigned char command) override {
        if (command == ENTER) {
            handleCommit();
            printLog(F("BaseItemZeroWidget::enter"), text);
            return true;
        }
        return false;
    }
};

#endif
