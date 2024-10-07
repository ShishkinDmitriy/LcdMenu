#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H

class LcdMenu;

/**
 * @class BaseWidget
 * @brief Base class for widget.
 */
class BaseWidget {
    template <typename T0, typename T1, typename T2>
    friend class Item;
    friend class BaseItemManyWidgets;
    friend class BaseItemSingleWidget;

  protected:
    /**
     * @brief Process a command decoded in 1 byte.
     * It can be a printable character or a control command like `ENTER` or `LEFT`.
     * Return value is used to determine operation was successful or ignored.
     * If the parent of item received that handle was ignored it can execute its own action on this command.
     * Thus, the item always has priority in processing; if it is ignored, it is delegated to the parent element.
     * Behaviour is very similar to Even Bubbling in JavaScript.
     * @param menu the owner menu of the item, can be used to retrieve required object, such as `DisplayInterface` or `MenuScreen`
     * @param command the character command, can be a printable character or a control command
     * @return true if command was successfully handled by item.
     */
    virtual bool process(LcdMenu* menu, unsigned char command) = 0;
    /**
     * @brief Draw the widget into specified buffer.
     *
     * @param buffer the buffer where widget will be drawn
     * @param size the number of symbols to draw, not necessary the same as buffer length.
     */
    virtual int draw(char* buffer, size_t size) = 0;
    /**
     * @brief Retrieve blinker offset.
     *
     * @return number of positions that need to move from the end of rendered
     * string to position a blinker.
     */
    virtual uint8_t getBlinkerOffset() = 0;

  public:
    virtual ~BaseWidget() = default;
};

#endif
