#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <manager.h>

TaskManager m;

void shutdown()
{
    m.shutdown();
}

void setup()
{
    // char buffer[64];
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(18, INPUT_PULLUP);
    setup_statements();
    start_oled_driver();
    attachInterrupt(digitalPinToInterrupt(18), shutdown, CHANGE);
    if (!Serial)
    {
        print_oled("Waiting for serial");
        delay(2000);
        if (!Serial)
        {
            print_oled("No serial found");
        }
    }

    if (!SD.begin())
    {
        print_oled("SD Card Error");
        Serial.println("SD Card Read Error");
        while (1)
            ;
    };

    m.add_task((char *)"test.m");
    // m.add_task((char *)"test3.m");
    m.run();

    Serial.println("End of the story");
    print_oled("The End");
    SD.end();
    while (true)
    {
        delay(120000);
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
}
