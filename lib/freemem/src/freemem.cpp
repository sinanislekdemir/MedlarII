#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


#include "freemem.h"

int freeMemory()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void pfm()
{
  Serial.print("Free mem=");
  Serial.println(freeMemory());
}
