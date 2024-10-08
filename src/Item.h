#ifndef ITEM_H
#define ITEM_H

#include "BaseItemManyWidgets.h"
#include "BaseItemSingleWidget.h"
#include "BaseItemZeroWidget.h"
#include "BaseWidgetValue.h"

template <typename T0 = void, typename T1 = void, typename T2 = void>
class Item final : public BaseItemManyWidgets {
  protected:
    void (*callback)(T0, T1, T2) = nullptr;
    void handleCommit() override {
        if (callback != nullptr) {
            callback(
                static_cast<BaseWidgetValue<T0>*>(widgets[0])->getValue(),
                static_cast<BaseWidgetValue<T1>*>(widgets[1])->getValue(),
                static_cast<BaseWidgetValue<T2>*>(widgets[2])->getValue());
        }
    }

  public:
    /**
     * @brief Construct a new Item Range Base object
     *
     * @param text The text of the item.
     * @param widget0 The first widget.
     * @param widget1 The second widget.
     * @param widget2 The third widget.
     * @param callback A pointer to the callback function to execute when this menu item is selected.
     */
    Item(
        const char* text,
        BaseWidgetValue<T0>* widget0,
        BaseWidgetValue<T1>* widget1,
        BaseWidgetValue<T2>* widget2,
        void (*callback)(T0, T1, T2))
        : BaseItemManyWidgets(text, new BaseWidget* [3] { widget0, widget1, widget2 }, 3), callback(callback) {}

    void setValues(T0 value0, T1 value1, T2 value2) {
        static_cast<BaseWidgetValue<T0>*>(widgets[0])->setValue(value0);
        static_cast<BaseWidgetValue<T1>*>(widgets[1])->setValue(value1);
        static_cast<BaseWidgetValue<T2>*>(widgets[2])->setValue(value2);
    }
};

template <typename T0, typename T1>
class Item<T0, T1, void> final : public BaseItemManyWidgets {
  protected:
    void (*callback)(T0, T1) = nullptr;
    void handleCommit() override {
        if (callback != nullptr) {
            callback(
                static_cast<BaseWidgetValue<T0>*>(widgets[0])->getValue(),
                static_cast<BaseWidgetValue<T1>*>(widgets[1])->getValue());
        }
    }

  public:
    Item(
        const char* text,
        BaseWidgetValue<T0>* widget0,
        BaseWidgetValue<T1>* widget1,
        void (*callback)(T0, T1))
        : BaseItemManyWidgets(text, new BaseWidget* [2] { widget0, widget1 }, 2), callback(callback) {}

    void setValues(T0 value0, T1 value1) {
        static_cast<BaseWidgetValue<T0>*>(widgets[0])->setValue(value0);
        static_cast<BaseWidgetValue<T1>*>(widgets[1])->setValue(value1);
    }
};

template <typename T0>
class Item<T0, void, void> final : public BaseItemSingleWidget {
  protected:
    void (*callback)(T0) = nullptr;

    void handleCommit() override {
        if (callback != nullptr) {
            callback(static_cast<BaseWidgetValue<T0>*>(this->widget)->getValue());
        }
    }

  public:
    Item(
        const char* text,
        BaseWidgetValue<T0>* widget,
        void (*callback)(T0))
        : BaseItemSingleWidget(text, widget), callback(callback) {}
    void setValues(T0 value0) {
        static_cast<BaseWidgetValue<T0>*>(this->widget)->setValue(value0);
    }
};

template <>
class Item<void, void, void> final : public BaseItemZeroWidget {
  protected:
    void (*callback)() = nullptr;

  public:
    Item(
        const char* text,
        void (*callback)())
        : BaseItemZeroWidget(text), callback(callback) {}

  protected:
    void handleCommit() override {
        if (callback != nullptr) {
            callback();
        }
    }
};

#endif
