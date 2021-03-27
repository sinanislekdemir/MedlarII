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
    // char *first = (char *)malloc(120);
    // char *back = (char *)malloc(20);
    // memset(first, 0, 120);
    // memset(back, 0, 20);
    // strcpy(first, "sinan islekdemir");
    // rest(first, 5, back);
    // Serial.println(back);
    // // Serial.println("ppp");
    // // Serial.println(back);
    // free(first);
    // free(back);
    // while(1){}
    // Serial.println("Test SD Card Ram Area");
    if (!SD.begin())
    {
        Serial.println("SD Card Read Error");
        while (1)
            ;
    };

    setup_statements();

    // MScript m;
    // m.open(0, "test2.m");
    // while(!m.finished)
    // {
    //     m.step();
    // }

    TaskManager m;
    m.add_task((char *)"test.m");
    m.add_task((char *)"test2.m");
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
