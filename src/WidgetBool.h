#ifndef ItemToggle_H
#define ItemToggle_H

#include "Widget.h"
#include <utils/utils.h>

/**
 * @brief Widget that allows user to toggle between ON/OFF states.
 * Widget has ON/OFF `enabled` state.
 */
template <>
class Widget<bool> : public BaseWidgetValue<bool> {

  protected:
    const char* textOn = nullptr;
    const char* textOff = nullptr;

  public:
    /**
     * @brief Construct a new Widget<bool> object.
     * @param value the initial enabled value
     * @param textOn display text when ON
     * @param textOff display text when OFF
     * @param format
     * @param blinkerOffset
     * @param callback reference to callback function
     */
    explicit Widget(const boolean value = false, const char* textOn = "ON", const char* textOff = "OFF", const char* format = "%s", const uint8_t blinkerOffset = 0, void (*callback)(bool) = nullptr)
        : BaseWidgetValue(value, format, blinkerOffset, callback), textOn(textOn), textOff(textOff) {}

    const char* getTextOn() const {
        return this->textOn;
    }

    const char* getTextOff() const {
        return this->textOff;
    }

  protected:
    int draw(char* buffer, const size_t size) override {
        return snprintf(buffer, size, format, value ? textOn : textOff);
    }
    /**
     * @brief Process command.
     *
     * Handle commands:
     * - `UP` - increment value and trigger callback;
     * - `DOWN` - decrement value and trigger callback;
     */
    bool process(LcdMenu* menu, const unsigned char command) override {
        if (command == ENTER) {
            value = !value;
            printLog(F("WidgetToggle::toggle"), value);
            if (callback != nullptr) {
                callback(value);
            }
            return true;
        }
        return false;
    }
};

#endif
