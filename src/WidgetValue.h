#ifndef WIDGET_VALUE_H
#define WIDGET_VALUE_H

#include "BaseWidget.h"
#include "printf.h"

template <typename T>
class WidgetValue : public BaseWidget {
    template <typename T0, typename T1, typename T2>
    friend class ItemControl;

  protected:
    T value;
    const T step;
    const char* format = nullptr;
    const uint8_t blinkerOffset = 0;
    void (*callback)(T) = nullptr;

  public:
    WidgetValue(T value, const T step, const char* format, const uint8_t blinkerOffset = 0, void (*callback)(T) = nullptr)
        : BaseWidget(), value(value), step(step), format(format), blinkerOffset(blinkerOffset), callback(callback) {}
    /**
     * @brief Retrieve current value.
     */
    T getValue() {
        return value;
    }
    /**
     * @brief Sets the value.
     *
     * @param newValue The value to set.
     * @note You need to call `LcdMenu::refresh` after this method to see the changes.
     */
    virtual void setValue(T newValue) {
        value = newValue;
    }
    /**
     * @brief Increment the value.
     */
    virtual bool increment() {
        value += step;
        printLog(F("WidgetValue::increment"), value);
        return true;
    }
    /**
     * @brief Decrement the value.
     */
    virtual bool decrement() {
        value -= step;
        printLog(F("WidgetValue::decrement"), value);
        return true;
    }

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
                        if (callback != nullptr) {
                            callback(value);
                        }
                    }
                    return true;
                case DOWN:
                    if (decrement()) {
                        if (callback != nullptr) {
                            callback(value);
                        }
                    }
                    return true;
                default:
                    return false;
            }
        }
        return false;
    }
    int draw(char* buffer, const size_t size) override {
        return snprintf_(buffer, size, format, value);
    }
    uint8_t getBlinkerOffset() override {
        return blinkerOffset;
    }
};

#endif
