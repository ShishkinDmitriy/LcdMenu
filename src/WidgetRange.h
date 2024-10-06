#ifndef WIDGET_RANGE_H
#define WIDGET_RANGE_H

#include "Widget.h"

template <typename T>
class WidgetRange : public Widget<T> {
  protected:
    const T minValue;
    const T maxValue;
    const bool cycle = false;

  public:
    WidgetRange(
        T value,
        const T step,
        const T min,
        const T max,
        const char* format,
        const bool cycle = false,
        const uint8_t blinkerOffset = 0,
        void (*callback)(T) = nullptr)
        : Widget<T>(value, step, format, blinkerOffset, callback),
          minValue(min),
          maxValue(max),
          cycle(cycle) {}
    /**
     * @brief Sets the value.
     * @param newValue The value to set.
     * @note You need to call `LcdMenu::refresh` after this method to see the changes.
     */
    void setValue(T newValue) override {
        Widget<T>::setValue(constrain(newValue, minValue, maxValue));
    }

  protected:
    /**
     * @brief Increments the value.
     * If value is out of range and `cycle` then `minValue` will be used.
     * If value is out of range and not `cycle` then do nothing.
     * @return true if incremented or reset (in case of cycle)
     */
    bool increment() override {
        if (this->value + this->step > maxValue) {
            if (cycle) {
                this->value = minValue;
                return true;
            }
            return false;
        }
        return Widget<T>::increment();
    }

    /**
     * @brief Decrements the value.
     * If value is out of range and `cycle` then `maxValue` will be used.
     * If value is out of range and not `cycle` then do nothing.
     * @return true if incremented or reset (in case of cycle)
     */
    bool decrement() override {
        if (this->value - this->step < minValue) {
            if (cycle) {
                this->value = maxValue;
                return true;
            }
            return false;
        }
        return Widget<T>::decrement();
    }
};

#endif
