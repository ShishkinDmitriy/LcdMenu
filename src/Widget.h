#ifndef WIDGET_VALUE_H
#define WIDGET_VALUE_H

#include "BaseWidgetWithValue.h"

template <typename T = void>
class Widget : public BaseWidgetWithValue<T> {

  protected:
    const T step;

  public:
    Widget(T value, const T step, const char* format, const uint8_t blinkerOffset = 0, void (*callback)(T) = nullptr)
        : BaseWidgetWithValue<T>(value, format, blinkerOffset, callback), step(step) {}

  protected:
    /**
     * @brief Process command.
     *
     * Handle commands:
     * - `UP` - increment value and trigger callback;
     * - `DOWN` - decrement value and trigger callback;
     */
    bool process(LcdMenu* menu, const unsigned char command) override {
        DisplayInterface* display = menu->getDisplay();
        if (display->getEditModeEnabled()) {
            switch (command) {
                case UP:
                    if (increment()) {
                        BaseWidgetWithValue<T>::triggerCallback();
                    }
                    return true;
                case DOWN:
                    if (decrement()) {
                        BaseWidgetWithValue<T>::triggerCallback();
                    }
                    return true;
                default:
                    return false;
            }
        }
        return false;
    }
    /**
     * @brief Increment the value.
     */
    virtual bool increment() {
        this->value += step;
        printLog(F("Widget::increment"), this->value);
        return true;
    }
    /**
     * @brief Decrement the value.
     */
    virtual bool decrement() {
        this->value -= step;
        printLog(F("Widget::decrement"), this->value);
        return true;
    }
};

template <>
class Widget<char*> : public BaseWidgetWithValue<char*> {

  public:
    explicit Widget(char* value, const char* format = "%s", const uint8_t blinkerOffset = 0, void (*callback)(char*) = nullptr)
        : BaseWidgetWithValue(value, format, blinkerOffset, callback) {}

  protected:
    bool process(LcdMenu* menu, unsigned char command) override = 0;
};

#endif
