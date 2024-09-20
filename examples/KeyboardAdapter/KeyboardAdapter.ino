/*
 * Usage example of `KeyboardAdapter`.
 */
#define DEBUG

#include <ItemInputCharset.h>
#include <LcdMenu.h>
#include <MenuScreen.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <input/KeyboardAdapter.h>

#define LCD_ROWS 2
#define LCD_COLS 16

// Create your charset
const char* charset = "0123456789";

// Declare the call back function
void inputCallback(char* value);

// clang-format off
MENU_SCREEN(mainScreen, mainItems,
    ITEM_INPUT_CHARSET("Con", "0123456", charset, inputCallback),
    ITEM_BASIC("Connect to WiFi"),
    ITEM_BASIC("Blink SOS"),
    ITEM_BASIC("Blink random"));
// clang-format on

LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2CAdapter lcdAdapter(&lcd, LCD_COLS, LCD_ROWS);
LcdMenu menu(lcdAdapter);
KeyboardAdapter keyboard(&menu, &Serial);

void setup() {
    Serial.begin(9600);
    lcdAdapter.begin();
    menu.setScreen(mainScreen);
}

void loop() {
    keyboard.observe();
}
/**
 * Define callback
 */
void inputCallback(char* value) {
    // Do stuff with value
    Serial.print(F("# "));
    Serial.println(value);
}
