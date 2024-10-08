#ifndef BASE_WIDGET_WITH_VALUE_H
#define BASE_WIDGET_WITH_VALUE_H

#include <BaseWidget.h>

class LcdMenu;

/**
 * @class BaseWidgetValue
 * @brief Base class for widget holding some value.
 */
template <typename T>
class BaseWidgetValue : public BaseWidget {
    template <typename T0, typename T1, typename T2>
    friend class Item;

  protected:
    T value;
    const char* format = nullptr;
    const uint8_t blinkerOffset = 0;
    void (*callback)(T) = nullptr;

  public:
    BaseWidgetValue(T value, const char* format, const uint8_t blinkerOffset = 0, void (*callback)(T) = nullptr)
        : BaseWidget(), value(value), format(format), blinkerOffset(blinkerOffset), callback(callback) {}
    /**
     * @brief Retrieve current value.
     */
    T getValue() const {
        return value;
    }
    /**
     * @brief Sets the value.
     *
     * @param newValue The value to set.
     * @note You need to call `LcdMenu::refresh` after this method to see the changes.
     */
    virtual void setValue(const T newValue) {
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
        return snprintf(buffer, size + 1, format, value);
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

    void handleChange() {
        if (callback != nullptr) {
            callback(value);
        }
    }

  public:
    ~BaseWidgetValue() override = default;
};

#endif
