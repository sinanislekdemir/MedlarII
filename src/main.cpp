#include "terminal.h"
#include <Arduino.h>
#include <SPI.h>
#include <manager.h>
#include <mdisplay.h>
#include <statements.h>
// #include <PS2Keyboard.h>

void setup()
{
    init_terminal();
}

void loop()
{
    // probably the terminal has crashed
    if (!Serial)
    {
        Serial.begin(9600);
    }
    Serial.println("System has encountered a terrible problem");
    // Now lets just blink the led13
    pinMode(LED_BUILTIN, OUTPUT);
    while (1)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}
