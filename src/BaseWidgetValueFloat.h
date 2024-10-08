#ifndef BASE_WIDGET_WITH_VALUE_FLOAT_H
#define BASE_WIDGET_WITH_VALUE_FLOAT_H

#include <BaseWidgetValue.h>
#include <printf.h>

class LcdMenu;

/**
 * @class BaseWidgetValue
 * @brief Base class for widget holding some value.
 */
template <>
class BaseWidgetValue<float> : public BaseWidget {
    template <typename T0, typename T1, typename T2>
    friend class Item;

  protected:
    float value;
    const char* format = nullptr;
    const uint8_t blinkerOffset = 0;
    void (*callback)(float) = nullptr;

  public:
    BaseWidgetValue(float value, const char* format, const uint8_t blinkerOffset = 0, void (*callback)(float) = nullptr)
        : BaseWidget(), value(value), format(format), blinkerOffset(blinkerOffset), callback(callback) {}
    /**
     * @brief Retrieve current value.
     */
    float getValue() const {
        return value;
    }
    /**
     * @brief Sets the value.
     *
     * @param newValue The value to set.
     * @note You need to call `LcdMenu::refresh` after this method to see the changes.
     */
    virtual void setValue(const float newValue) {
        value = newValue;
    }

  protected:
    /**
     * @brief Draw the widget into specified buffer.
     *
     * @param buffer the buffer where widget will be drawn
     * @param size the number of symbols to draw, not necessary the same as buffer length.
     */
    int draw(char* buffer, const size_t size) override {
        return snprintf(buffer, size, format, value);
    }
    bool process(LcdMenu* menu, unsigned char command) override = 0;
    /**
     * @brief Retrieve blinker offset.
     *
     * @return number of positions that need to move from the end of rendered
     * string to position a blinker.
     */
    uint8_t getBlinkerOffset() override {
        return blinkerOffset;
    }

    void triggerCallback() {
        if (callback != nullptr) {
            callback(value);
        }
    }

  public:
    ~BaseWidgetValue() override = default;
};

#endif
