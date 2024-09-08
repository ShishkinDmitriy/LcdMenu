/**
 * ---
 *
 * # ItemInput
 *
 * This is an item type where a user can type in information,
 * the information is persisted in the item and can be gotten later by
 * using `item->value`
 */

#ifndef ItemInput_H
#define ItemInput_H

// Include the header file for the base class.
#include "MenuItem.h"
#include <utils/utils.h>

// Declare a class for menu items that allow the user to input information.
class ItemInput : public MenuItem {
   private:
    // Declare a string to hold the input value.
    char* value;

    // Declare a function pointer for the input callback.
    fptrStr callback;
    uint8_t windowMin = 0;
    // uint8_t windowMax = 0;
    uint8_t windowSize = 0;
    uint8_t currentPosition;
   public:
    /**
     * Construct a new ItemInput object with an initial value.
     *
     * @param text The text to display for the item.
     * @param value The initial value for the input.
     * @param callback A reference to the callback function to be invoked when
     * the input is submitted.
     */
    ItemInput(const char* text, char* value, fptrStr callback)
        : MenuItem(text, MENU_ITEM_INPUT), value(value), callback(callback) {}

    /**
     * Construct a new ItemInput object with no initial value.
     *
     * @param text The text to display for the item.
     * @param callback A reference to the callback function to be invoked when
     * the input is submitted.
     */
    ItemInput(const char* text, fptrStr callback)
        : ItemInput(text, (char*)"", callback) {}

    /**
     * Get the current input value for this item.
     *
     * @return The current input value as a string.
     */
    char* getValue() { return value; }

    /**
     * Set the input value for this item.
     *
     * @param value The new input value.
     */
    void setValue(char* value) { this->value = value; }

    /**
     * Get the callback function for this item.
     *
     * @return The function pointer to the callback function.
     */
    fptrStr getCallbackStr() { return callback; }

    void enter(DisplayInterface* display) override {
        if (display->getEditModeEnabled()) {
            return;
        }
        uint8_t size = display->getMaxCols() - (strlen(text) + 2) - 1;
        currentPosition = 0;
        windowMin = 0;
        windowSize = size;
        MenuItem::draw(display);
        display->setEditModeEnabled(true);
        display->resetBlinker(constrainBlinkerPosition(strlen(text) + 2, display->getMaxCols()));
        display->drawBlinker();
    };

    void back(DisplayInterface* display) override {
        display->setEditModeEnabled(false);
        display->clearBlinker();
        if (callback != NULL) {
            callback(value);
        }
    };

    void left(DisplayInterface* display) override {
        if (windowMin == 0 && currentPosition == 0) {
            return;
        }
        currentPosition--;
        if (currentPosition < windowMin) {
            windowMin--;
            MenuItem::draw(display);
        }
        display->resetBlinker(constrainBlinkerPosition(display->getBlinkerCol() - 1, display->getMaxCols()));
        // Log
        printCmd(F("LEFT"), value[display->getBlinkerCol() - (strlen(text) + 2)]);
    };

    void right(DisplayInterface* display) override {
        uint8_t ub = strlen(value);
        if ((windowMin + windowSize - 1) == ub && currentPosition == ub) {
            return;
        }
        currentPosition++;
        if (currentPosition > (windowMin + windowSize - 1)) {
            windowMin++;
            MenuItem::draw(display);
        }
        display->resetBlinker(constrainBlinkerPosition(display->getBlinkerCol() + 1, display->getMaxCols()));
        // Log
        printCmd(F("RIGHT"), value[display->getBlinkerCol() - (strlen(text) + 2)]);
    };

    uint8_t constrainBlinkerPosition(uint8_t blinkerPosition, uint8_t maxCols) {
        //
        // calculate lower and upper bound
        //
        uint8_t lb = strlen(text) + 2;
        uint8_t ub = lb + strlen(value);
        ub = constrain(ub, lb, maxCols - 2);
        //
        // set cursor position
        //
        return constrain(blinkerPosition, lb, ub);
    }

    void draw(DisplayInterface* display, uint8_t row) override {
        uint8_t maxCols = display->getMaxCols();
        static char* buf = new char[maxCols];
        if (windowSize == 0) {
            substring(value, 0, maxCols - (strlen(text) + 2) - 1, buf);
        } else {
            substring(value, windowMin, windowSize, buf);
        }
        display->drawItem(row, text, ':', buf);
    }

    /**
     * Execute a "backspace cmd" on menu
     *
     * *NB: Works only for `ItemInput` type*
     *
     * Removes the character at the current cursor position.
     */
    void backspace(DisplayInterface* display) override {
        if (strlen(value) == 0 || currentPosition == 0) {
            return;
        }
        // uint8_t p = display->getBlinkerCol() - (strlen(text) + 2) - 1;
        remove(value, currentPosition - 1, 1);
        printCmd(F("BACKSPACE"), value);
        currentPosition--;
        if (currentPosition < windowMin) {
            windowMin--;
        }
        MenuItem::draw(display);
        display->resetBlinker(constrainBlinkerPosition(display->getBlinkerCol() - 1, display->getMaxCols()));
    }

    /**
     * Display text at the cursor position
     * used for `Input` type menu items
     * @param character character to append
     */
    void type2(DisplayInterface* display, const char character) override {
        //
        // calculate lower and upper bound
        //
        uint8_t length = strlen(value);
        uint8_t lb = strlen(text) + 2;
        uint8_t ub = lb + length;
        uint8_t maxCols = display->getMaxCols();
        ub = constrain(ub, lb, maxCols - 2);
        printCmd(F("TYPE-CHAR-l"), length);
        printCmd(F("TYPE-CHAR-lb"), lb);
        printCmd(F("TYPE-CHAR-ub"), ub);
        printCmd(F("TYPE-CHAR-bl"), display->getBlinkerCol());
        //
        // update text
        //
        if (currentPosition < length) {
            char start[length];
            char end[length];
            char* joined = new char[length + 2];
            substring(value, 0, currentPosition, start);
            substring(value, currentPosition, length - currentPosition, end);
            concat(start, character, end, joined);
            value = joined;
        } else {
            char* buf = new char[length + 2];
            concat(value, character, buf);
            value = buf;
        }
        currentPosition++;
        if (currentPosition > (windowMin + windowSize - 1)) {
            windowMin++;
        }
        MenuItem::draw(display);
        display->resetBlinker(constrainBlinkerPosition(display->getBlinkerCol() + 1, display->getMaxCols()));
        // Log
        printCmd(F("TYPE-CHAR"), character);
    }

    /**
     * Clear the value of the input field
     */
    void clear(DisplayInterface* display) override {
        //
        // set the value
        //
        value = (char*)"";
        // Log
        printCmd(F("CLEAR"), value);
        //
        // update blinker position
        //
        MenuItem::draw(display);
        display->resetBlinker(constrainBlinkerPosition(strlen(text) + 2, display->getMaxCols()));
    }

};

#define ITEM_INPUT(...) (new ItemInput(__VA_ARGS__))

#endif  // ITEM_INPUT_H
