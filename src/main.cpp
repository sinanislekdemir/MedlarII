#include <Arduino.h>
#include <SPI.h>
#include <manager.h>

TaskManager m;

void shutdown()
{
    m.shutdown();
}

void setup()
{
    Serial.begin(9600);
    setup_statements();
    start_vga_driver();
    // f_delay(64000);
    pinMode(18, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(18), shutdown, CHANGE);
    if (!Serial)
    {
        println_vga("Waiting for serial");
        f_delay(2000);
        if (!Serial)
        {
            println_vga("No serial found");
        }
    }

    if (!SD.begin())
    {
        println_vga("SD Card Error");
        Serial.println("SD Card Read Error");
        while (1)
            ;
    };

    m.add_task((char *)"sread.m");
    m.add_task((char *)"test2.m");
    m.run();

    Serial.println("End of the story");
    println_vga("The End");
    SD.end();
}

void loop()
{
}
