#include <Arduino.h>
#include <sram.h>

SRam ram;

void setup() {
  char buffer[64];
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){}

  Serial.println("Test SD Card Ram Area");
  if (!SD.begin()) {
    Serial.println("Error: Hay mk");
    while(1);
  };

  Serial.println(sizeof(memoryBlockHeader));

  ram.open("ram.dat");
  Serial.println("Create first variable");
  ram.allocateVariable((char *)"str_1", 0, 1024);
  ram.allocateVariable((char *)"str_2", 0, 1024);
  ram.allocateVariable((char *)"str_3", 0, 1024);
  ram.write((char *)"str_1", 0, 0, (char *)"Huysuz Adam!", 12);
  ram.write((char *)"str_2", 0, 0, (char *)"Sinan Islekdemir", 16);
  ram.write((char *)"str_3", 0, 0, (char *)"test", 4);

  // lets see if we can read it back
  ram.read((char *)"str_1", 0, 0, buffer, 64);
  Serial.print((char *)"str_1 = ");
  Serial.println(buffer);

  Serial.print((char *)"str_2 = ");
  ram.read((char *)"str_2", 0, 0, buffer, 64);
  Serial.println(buffer);

  Serial.print((char *)"str_3 = ");
  ram.read((char *)"str_3", 0, 0, buffer, 64);
  Serial.println(buffer);

  // let see if we can delete the variable
  ram.deleteVariable((char *)"str_2", 0);
  uint16_t bytes_read = ram.read((char *)"str_2", 0, 0, buffer, 64);
  if (bytes_read == 0) {
    Serial.println("Variable is deleted");
  } else {
    Serial.println("Not deleted");
  }

  Serial.print((char *)"str_3 = ");
  ram.read((char *)"str_3", 0, 0, buffer, 64);
  Serial.println(buffer);

  ram.close();
  Serial.println("End of the story");
  SD.end();
}

void loop() {
  // put your main code here, to run repeatedly:
}
