#ifndef BASE_ITEM_SINGLE_WIDGET_H
#define BASE_ITEM_SINGLE_WIDGET_H

#include "LcdMenu.h"
#include "MenuItem.h"
#include "Widget.h"
#include <utils/utils.h>

#define ITEM_DRAW_BUFFER_SIZE 20

class BaseItemSingleWidget : public MenuItem {
protected:
    BaseWidget* widget = nullptr;

  public:
    BaseItemSingleWidget(const char* text, BaseWidget* widget)
        : MenuItem(text), widget(widget) {}

  protected:
    virtual void triggerCallback() = 0;
    static void log(const char* buffer, const size_t size, const int idx) {
        // Serial.print("buffer [");
        // for (size_t i = 0; i < size + 1; i++) {
        //     Serial.print(buffer[i]);
        // }
        // Serial.println("]");
        // Serial.print("        ");
        // for (size_t i = 0; i < idx; i++) {
        //     Serial.print(" ");
        // }
        // Serial.print("^ ");
        // Serial.print("idx = ");
        // Serial.println(idx);
    }
    static void logBuf(const char* buffer) {
        // Serial.print("buf [");
        // Serial.print(buffer);
        // Serial.println("]");
    }
    /**
     * @details In case of not in edit mode then draw from left to right until
     * have space of widgets. In case of edit mode the first priority is to show
     * the blinker. If widget has long prefix or suffix they should not waste space.
     * For example, we have limit of 10 chars, and we have 2 widgets:
     * - "+2,"
     * - "12.3dB" <- is current
     *
     * ```
     *   0 1 2 3 4 5 6 7 8 9 10  <-  index
     *   _ _ _ _ _ _ _ _ _ _ _   (1) Initialize buffer of size N + 1 for last \0 char
     *   _ _ _ _ _ _ 1 2 . 3 _   (2) Append current widget value "12.3" to the end
     *   _ _ _ + 2 , 1 2 . 3 _   (3) If still has space, prepend previous widgets
     *   + 2 , 1 2 . 3 _ _ _ _   (4) If still has space, shift to the beginning
     *   + 2 , 1 2 . 3 d B _ _   (5) If still has space, append current suffix
     *   + 2 , 1 2 . 3 d B \0_   (6) Append \0 char, if no space it will be on index N + 1
     * ```
     */
    void draw(DisplayInterface* display, const uint8_t row) override {
        const uint8_t maxCols = display->getMaxCols() - 1;
        uint8_t col = 1;
        col += display->drawAt(row, col, const_cast<char*>(this->text));
        col += display->drawAt(row, col, ':');
        const uint8_t limit = maxCols - col;
        char buffer[limit + 1];           // Hold whole row rendered + 1 char for \0
        char buf[ITEM_DRAW_BUFFER_SIZE];  // Hold single widget rendered
        size_t length = 0;                // Hold single widget rendered length
        int idx = 0;                      // index of first writable char
        uint8_t blinkerPosition = 0;      // Position of blinker
        logBuf(buf);
        if (!display->getEditModeEnabled()) {
            length = widget->draw(buf, min(ITEM_DRAW_BUFFER_SIZE, limit - idx));
            logBuf(buf);
            // E.g. widgets[current]->draw = "+20gr"
            // idx = 6, length = 6
            // 0 1 2 3 4 5 6 7 8 9 10
            // 1 2 . 3 d B _ _ _ _ \0
            //             ^ idx
            strncpy(buffer + idx, buf, length);
            idx += static_cast<int>(length);
            log(buffer, limit, idx);
            // 0 1 2 3 4 5 6 7 8 9 10
            // 1 2 . 3 d B + 2 0 g \0
            //                     ^ idx
        } else {
            //==========================================================
            //  Draw current widget up to blinker
            //==========================================================
            // Serial.println("Draw current widget up to blinker...");
            const uint8_t suffixSize = widget->getBlinkerOffset();
            // e.g. widgets[current]->draw = "12.3dB"
            // offset = 2, length = 6 -> 4
            length = widget->draw(buf, ITEM_DRAW_BUFFER_SIZE) - suffixSize;
            logBuf(buf);
            // Save `dB` into suffix
            char suffix[suffixSize];
            strncpy(suffix, buf + length, suffixSize);
            // Draw widget into buffer
            length = min(length, limit);
            strncpy(buffer + limit - length, buf, length);
            idx = limit - static_cast<int>(length);
            log(buffer, limit, idx);
            // 0 1 2 3 4 5 6 7 8 9 10
            // _ _ _ _ _ _ 1 2 . 3 \0
            //             ^ idx
            if (idx > 0) {
                //==========================================================
                //  Shift all to beginning
                //==========================================================
                // Serial.println("Shift all to beginning...");
                // Shift all to beginning
                // 0 1 2 3 4 5 6 7 8 9 10
                // _ _ _ _ _ _ 1 2 . 3 \0
                //             ^ idx
                for (int i = idx; i < limit; i++) {
                    buffer[i - idx] = buffer[i];
                }
                // 0 1 2 3 4 5 6 7 8 9 10
                // 1 2 . 3 _ _ 1 2 . 3 \0
                //             ^ idx
                idx = limit - idx;
                blinkerPosition = idx - 1;
                log(buffer, limit, idx);
                // 0 1 2 3 4 5 6 7 8 9 10
                // 1 2 . 3 _ _ 1 2 . 3 \0
                //         ^ idx
            } else {
                idx = limit;
                blinkerPosition = idx - 1;
            }
            if (idx > 0 && idx < limit) {
                //==========================================================
                //  Draw suffix
                //==========================================================
                // Serial.println("Draw suffix...");
                // 0 1 2 3 4 5 6 7 8 9 10
                // 1 2 . 3 _ _ _ _ _ _ \0
                //         ^ idx
                length = min(suffixSize, limit - idx);
                strncpy(buffer + idx, suffix, length);
                idx += static_cast<int>(length);
                log(buffer, limit, idx);
                // 0 1 2 3 4 5 6 7 8 9 10
                // 1 2 . 3 d B _ _ _ _ \0
                //             ^ idx
            }
        }
        // Fill remaining wih spaces
        for (int i = idx; i < limit; i++) {
            buffer[i] = ' ';
        }
        buffer[limit] = '\0';
        // Serial.println("Final");
        log(buffer, limit, idx);
        display->drawAt(row, col, buffer);
        if (display->getEditModeEnabled()) {
            display->drawBlinker();
            display->resetBlinker(col + blinkerPosition);
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
                triggerCallback();
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

#endif
