#ifndef BASE_ITEM_MANY_H
#define BASE_ITEM_MANY_H

#include "LcdMenu.h"
#include "MenuItem.h"
#include "Widget.h"
#include <utils/utils.h>

class BaseItemMany : public MenuItem {
  protected:
    BaseWidget** widgets = nullptr;
    const uint8_t size = 0;
    uint8_t current = 0;

  public:
    BaseItemMany(const char* text, BaseWidget** widgets, const uint8_t size, uint8_t current = 0)
        : MenuItem(text), widgets(widgets), size(size), current(current) {}
    uint8_t getCurrent() const {
        return current;
    }
    void setCurrent(const uint8_t newCurrent) {
        current = newCurrent;
    }

  protected:
    void reset() {
        current = 0;
    }
    void draw(DisplayInterface* display, uint8_t row) override {
        const uint8_t maxCols = display->getMaxCols() - 1;
        uint8_t col = 1;
        col += display->drawAt(row, col, (char*)this->text);
        col += display->drawAt(row, col, ':');
        const uint8_t limit = maxCols - col;
        // Serial.print("Call draw on (row=");
        // Serial.print(row);
        // Serial.print(", col=");
        // Serial.print(col);
        // Serial.print(", limit=");
        // Serial.print(limit);
        // Serial.println(")");
        char buffer[limit];
        buffer[0] = '\0';  // Cleanup
        uint8_t accum = 0;
        int rightTrim = -1;
        uint8_t blinkerPosition = 0;
        if (!display->getEditModeEnabled()) {
            for (size_t i = 0; i < size; i++) {
                char text[20];
                accum += widgets[i]->draw(text, 20);
                rightTrim = accum - limit;
                // Serial.print("Right iteration [");
                // Serial.print(i);
                // Serial.print("], text=[");
                // Serial.print(text);
                // Serial.print("], rightTrim=[");
                // Serial.print(rightTrim);
                // Serial.println("]");
                if (rightTrim >= 0) {
                    // Serial.print("rightTrim exceed ");
                    // Serial.println(rightTrim);
                    substring(text, 0, strlen(text) - rightTrim, text);
                    concat(buffer, text, buffer);
                    break;
                }
                concat(buffer, text, buffer);
            }
        } else {
            char texts[size][20];
            int left = current;
            int right = current + 1;
            int leftTrim = -1;
            // From current to left
            // Serial.println("Left iteration start");
            while (left >= 0 && leftTrim < 0) {
                accum += widgets[left]->draw(texts[left], 20);
                leftTrim = accum - limit;
                // Serial.print("Left iteration [");
                // Serial.print(left);
                // Serial.print("], texts[left]=[");
                // Serial.print(texts[left]);
                // Serial.print("], leftTrim=[");
                // Serial.print(leftTrim);
                // Serial.println("]");
                left--;
            }
            // Serial.println("Left iteration end");
            if (leftTrim < 0 && current < size - 1) {
                // From current + 1 to right
                // Serial.println("Right iteration start");
                while (right < size && rightTrim < 0) {
                    accum += widgets[right]->draw(texts[right], 20);
                    rightTrim = accum - limit;
                    // Serial.print("Right iteration [");
                    // Serial.print(right);
                    // Serial.print("], texts[right]=[");
                    // Serial.print(texts[right]);
                    // Serial.print("], rightTrim=[");
                    // Serial.print(rightTrim);
                    // Serial.println("]");
                    right++;
                }
                // Serial.println("Right iteration end");
            }
            // Serial.println("Constructing...");
            // Serial.print("left=");
            // Serial.print(left);
            // Serial.print(", right=");
            // Serial.println(right);
            for (int i = left + 1; i < right; i++) {
                char* text = texts[i];
                // Serial.print("Handle (");
                // Serial.print(i);
                // Serial.print(") ");
                // Serial.println(text);
                if (i == left + 1 && leftTrim > 0) {
                    substring(text, leftTrim, strlen(text) - leftTrim, text);
                    // Serial.print("After left substring [");
                    // Serial.print(text);
                    // Serial.println("]");
                }
                if (i == right - 1 && rightTrim > 0) {
                    substring(text, 0, strlen(text) - rightTrim, text);
                    // Serial.print("After right substring [");
                    // Serial.print(text);
                    // Serial.println("]");
                }
                concat(buffer, text, buffer);
                if (i == current) {
                    // Serial.print("Current widget, buffer=[");
                    // Serial.print(buffer);
                    // Serial.print("], offset=");
                    // Serial.println(widgets[i]->getBlinkerOffset());
                    blinkerPosition = strlen(buffer) - widgets[i]->getBlinkerOffset() - 1;
                    // Serial.print("blinker global offset = ");
                    // Serial.println(blinkerPosition);
                }
            }
        }
        Serial.print("Constructed [");
        Serial.print(buffer);
        Serial.println("]");
        display->drawAt(row, col, buffer);
        if (rightTrim < 0) {
            Serial.print("Call clearAfter ");
            Serial.println(col + strlen(buffer));
            display->clearAfter(row, col + strlen(buffer) - 1);
        }
        if (display->getEditModeEnabled()) {
            display->drawBlinker();
            // Serial.print("blinkerPosition = ");
            // Serial.println(col + blinkerPosition);
            display->resetBlinker(col + blinkerPosition);
        }
        // Serial.println("");
        // Serial.println("");
    }

    virtual void triggerCallback() = 0;

    bool process(LcdMenu* menu, const unsigned char command) override {
        DisplayInterface* display = menu->getDisplay();
        if (widgets[current]->process(menu, command)) {
            MenuItem::draw(display);
            return true;
        }
        if (display->getEditModeEnabled()) {
            switch (command) {
                case ENTER:
                    if (current == this->size - 1) {
                        back(display);
                        return true;
                    }
                    // Be aware, no break statement.
                    // if ENTER on last widget, then behaves like BACK
                    // if ENTER on other widgets, then behaves like RIGHT
                case RIGHT:
                    current = (current + 1) % this->size;
                    MenuItem::draw(display);
                    return true;
                case LEFT:
                    current--;
                    if (current < 0) {
                        current = this->size;
                    }
                    MenuItem::draw(display);
                    return true;
                case BACK:
                    back(display);
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
    void back(DisplayInterface* display) {
        display->setEditModeEnabled(false);
        reset();
        triggerCallback();
        display->clearBlinker();
        MenuItem::draw(display);
        printLog(F("ItemControl::exitEditMode"), this->text);
    }
};

#endif
