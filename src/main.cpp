#include <Arduino.h>
#include <SPI.h>
#include "terminal.h"
#include <statements.h>
#include <mdisplay.h>
#include <manager.h>
// #include <PS2Keyboard.h>

void setup()
{
    // Serial.begin(9600);
    // TaskManager m;
    // setup_statements();
    // start_vga_driver();
    // pinMode(SHUTDOWN_PIN, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(SHUTDOWN_PIN), shutdown, CHANGE);
    // if (!Serial)
    // {
    //     println_vga(MSG_SERIAL_ERR);
    // }

    // if (!SD.begin())
    // {
    //     Serial.println("man");
    //     while(1);
    // }
    // m.add_task("test.m");
    // m.run();
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
    while(1)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}
