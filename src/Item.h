#ifndef ITEM_H
#define ITEM_H

#include "BaseItemMany.h"
#include "LcdMenu.h"
#include "MenuItem.h"
#include "Widget.h"
#include <utils/utils.h>

template <typename T0 = void, typename T1 = void, typename T2 = void>
class Item final : public BaseItemMany {
  protected:
    void (*callback)(T0, T1, T2) = nullptr;
    void triggerCallback() override {
        if (callback != nullptr) {
            callback(
                static_cast<BaseWidgetWithValue<T0>*>(widgets[0])->getValue(),
                static_cast<BaseWidgetWithValue<T1>*>(widgets[1])->getValue(),
                static_cast<BaseWidgetWithValue<T2>*>(widgets[2])->getValue());
        }
    }

  public:
    /**
     * @brief Construct a new Item Range Base object
     *
     * @param text The text of the item.
     * @param widget0 The first widget.
     * @param widget1 The second widget.
     * @param widget2 The third widget.
     * @param callback A pointer to the callback function to execute when this menu item is selected.
     */
    Item(
        const char* text,
        BaseWidgetWithValue<T0>* widget0,
        BaseWidgetWithValue<T1>* widget1,
        BaseWidgetWithValue<T2>* widget2,
        void (*callback)(T0, T1, T2))
        : BaseItemMany(text, new BaseWidget* [3] { widget0, widget1, widget2 }, 3), callback(callback) {}

    void setValues(T0 value0, T1 value1, T2 value2) {
        static_cast<BaseWidgetWithValue<T0>*>(widgets[0])->setValue(value0);
        static_cast<BaseWidgetWithValue<T1>*>(widgets[1])->setValue(value1);
        static_cast<BaseWidgetWithValue<T2>*>(widgets[2])->setValue(value2);
    }
};

template <typename T0, typename T1>
class Item<T0, T1, void> final : public BaseItemMany {
  protected:
    void (*callback)(T0, T1) = nullptr;

    void triggerCallback() override {
        if (callback != nullptr) {
            callback(
                static_cast<BaseWidgetWithValue<T0>*>(widgets[0])->getValue(),
                static_cast<BaseWidgetWithValue<T1>*>(widgets[1])->getValue());
        }
    }

  public:
    Item(
        const char* text,
        BaseWidgetWithValue<T0>* widget0,
        BaseWidgetWithValue<T1>* widget1,
        void (*callback)(T0, T1))
        : BaseItemMany(text, new BaseWidget* [2] { widget0, widget1 }, 2), callback(callback) {}

    void setValues(T0 value0, T1 value1) {
        static_cast<BaseWidgetWithValue<T0>*>(widgets[0])->setValue(value0);
        static_cast<BaseWidgetWithValue<T1>*>(widgets[1])->setValue(value1);
    }
};

template <typename T0>
class Item<T0, void, void> final : public MenuItem {
  protected:
    BaseWidgetWithValue<T0>* widget = nullptr;
    void (*callback)(T0) = nullptr;

  public:
    Item(
        const char* text,
        BaseWidgetWithValue<T0>* widget,
        void (*callback)(T0))
        : MenuItem(text), widget(widget), callback(callback) {}
    void setValues(T0 value0) {
        widget->setValue(value0);
    }

  protected:
    void draw(DisplayInterface* display, const uint8_t row) override {
        const uint8_t maxCols = display->getMaxCols() - 1;
        uint8_t col = 1;
        col += display->drawAt(row, col, (char*)this->text);
        col += display->drawAt(row, col, ':');
        const uint8_t limit = maxCols - col;
        char buf[limit];
        widget->draw(buf, limit);
        col += display->drawAt(row, col, buf);
        Serial.print("clearAfter (row=");
        Serial.print(row);
        Serial.print(", col=");
        Serial.print(col);
        Serial.println(")");
        display->clearAfter(row, col);
        if (display->getEditModeEnabled()) {
            display->drawBlinker();
            display->resetBlinker(col - 1 - widget->getBlinkerOffset());
        }
    }

    bool process(LcdMenu* menu, const unsigned char command) override {
        DisplayInterface* display = menu->getDisplay();
        if (widget->process(menu, command)) {
            MenuItem::draw(display);
            return true;
        }
        if (display->getEditModeEnabled()) {
            switch (command) {
                case ENTER:
                case BACK:
                    display->setEditModeEnabled(false);
                    if (callback != nullptr) {
                        callback(widget->getValue());
                    }
                    display->clearBlinker();
                    printLog(F("ItemControl::exitEditMode"), this->text);
                    return true;
                default:
                    return false;
            }
        } else {
            switch (command) {
                case ENTER:
                    display->setEditModeEnabled(true);
                    printLog(F("ItemControl::enterEditMode"), this->text);
                    MenuItem::draw(display);
                    return true;
                default:
                    return false;
            }
        }
    }
};

template <>
class Item<void, void, void> final : public MenuItem {
  protected:
    void (*callback)() = nullptr;

  public:
    Item(
        const char* text,
        void (*callback)())
        : MenuItem(text), callback(callback) {}

  protected:
    void draw(DisplayInterface* display, const uint8_t row) override {
        size_t len = strlen(this->text);
        display->drawAt(row, 1, (char*)this->text);
        display->clearAfter(row, 1 + len);
    }

    bool process(LcdMenu* menu, const unsigned char command) override {
        switch (command) {
            case ENTER:
                if (callback != nullptr) {
                    callback();
                }
                printLog(F("MenuItem<>::enter"), text);
                return true;
            default:
                return false;
        }
    }
};

#endif
