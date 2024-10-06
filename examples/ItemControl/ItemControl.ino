#include <Button.h>
#include <Item.h>
#include <LcdMenu.h>
#include <MenuScreen.h>
#include <SimpleRotary.h>
#include <WidgetBool.h>
#include <WidgetFloat.h>
#include <WidgetRange.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <input/ButtonAdapter.h>
#include <input/KeyboardAdapter.h>
#include <input/SimpleRotaryAdapter.h>

#define LCD_ROWS 2
#define LCD_COLS 12

// Initialize the main menu items
// clang-format off
MENU_SCREEN(mainScreen, mainItems,
    new Item<>("Command",
        [] {
            Serial.println("callback0()");
        }),
    new Item<bool>("Temp",
        new Widget<bool>(false),
        [](const bool v0) {
            Serial.print("callback1(");
            Serial.print(v0);
            Serial.println(")");
        }),
    new Item<uint8_t>("Price",
        new Widget<uint8_t>(1, 1, "$%u"),
        [](const uint8_t v0) {
            Serial.print("callback1(");
            Serial.print(v0);
            Serial.println(")");
        }),
    new Item<char>("Char",
        new WidgetRange<char>('A', 1, 'A', 'Z', "[%c]", true, 1),
        [](const char v0) {
            Serial.print("callback1(");
            Serial.print(v0);
            Serial.println(")");
    }),
    new Item<char>("Char",
        new WidgetRange<char>('a', 1, 'a', 'z', "%c", true, 0),
        [](const char v0) {
            Serial.print("callback1(");
            Serial.print(v0);
            Serial.println(")");
        }),
    new Item<float>("Temp",
        new WidgetRange<float>(2.5, 0.5, -30.0, 40.0, "%+.1f\002C", false, 2),
        [](const float v0) {
            Serial.print("callback1(");
            Serial.print(v0);
            Serial.println(")");
        }),
    new Item<float, int>("Dist",
        new WidgetRange<float>(1.0, 0.1, 0.0, 10.0, "%2.1fm", false, 1),
        new WidgetRange<int>(10, 1, 0, 100, "\003%d%%", false, 1),
        [](const float v0, const int v1) {
            Serial.print("callback2(");
            Serial.print(v0);
            Serial.print(", ");
            Serial.print(v1);
            Serial.println(")");
        }),
    new Item<int, int, int>("Time",
        new WidgetRange<int>(0, 1,0, 23,  "%02d", true),
        new WidgetRange<int>(0, 1,0, 59,  ":%02d", true),
        new WidgetRange<int>(0, 1,0, 59,  ":%02d", true),
        [](const int v0, const int v1, const int v2) {
            Serial.print("callback3(");
            Serial.print(v0);
            Serial.print(", ");
            Serial.print(v1);
            Serial.print(", ");
            Serial.print(v2);
            Serial.println(")");
        }),
    ITEM_BASIC("Blink random"));
// clang-format on

// Construct the LcdMenu
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2CAdapter lcdAdapter(&lcd, LCD_COLS, LCD_ROWS);
LcdMenu menu(lcdAdapter);
SimpleRotary encoder(2, 3, 4);
SimpleRotaryAdapter rotaryInput(&menu, &encoder);  // Rotary input adapter
Button backspaceBtn(5);
ButtonAdapter backspaceBtnA(&menu, &backspaceBtn, BACKSPACE);  // Push button for backspace
KeyboardAdapter keyboard(&menu, &Serial);

void setup() {
    backspaceBtn.begin();
    lcdAdapter.begin();
    Serial.begin(9600);
    menu.setScreen(mainScreen);
    byte celsius[8] = {B11100, B10100, B11100, B0000, B00000, B00000, B00000, B00000};
    byte plusMinus[8] = {B00000, B00100, B01110, B00100, B00000, B01110, B00000, B00000};
    lcd.createChar(2, celsius);
    lcd.createChar(3, plusMinus);
}

void loop() {
    rotaryInput.observe();
    backspaceBtnA.observe();
    keyboard.observe();
}
