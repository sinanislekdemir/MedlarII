#include "mdisplay.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int cursor_x = 0;
int cursor_y = 0;

int start_oled_driver()
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return -1;
    }
    display.clearDisplay();
    display.display();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);
    display.println("Testing");
    display.display();
    delay(1000);
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    return 0;
}

int println_oled(const char *text)
{
    display.setCursor(cursor_x, cursor_y);
    display.println(text);
    display.display();
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    if (cursor_y > 50) {
        display.clearDisplay();
        cursor_y = 0;
    }
    return 0;
}

int println_oled(double text)
{
    display.setCursor(cursor_x, cursor_y);
    display.println(text);
    display.display();
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    if (cursor_y > 50) {
        display.clearDisplay();
        cursor_y = 0;
    }
    return 0;
}

int print_oled(const char *text)
{
    display.setCursor(cursor_x, cursor_y);
    display.println(text);
    display.display();
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    if (cursor_y > 50) {
        display.clearDisplay();
        cursor_y = 0;
    }
    return 0;
}

int print_oled(double text)
{
    display.setCursor(cursor_x, cursor_y);
    display.println(text);
    display.display();
    cursor_x = display.getCursorX();
    cursor_y = display.getCursorY();
    if (cursor_y > 50) {
        display.clearDisplay();
        cursor_y = 0;
    }
    return 0;
}
