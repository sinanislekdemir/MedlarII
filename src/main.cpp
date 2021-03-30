#include <Arduino.h>
#include <manager.h>

void setup()
{
    // char buffer[64];
    // put your setup code here, to run once:
    Serial.begin(9600);
    while (!Serial)
    {
    }

    if (!SD.begin())
    {
        Serial.println("SD Card Read Error");
        while (1)
            ;
    };

    setup_statements();

    TaskManager m;
    m.add_task((char *)"test2.m");
    m.add_task((char *)"test3.m");
    m.run();

    Serial.println("End of the story");
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
