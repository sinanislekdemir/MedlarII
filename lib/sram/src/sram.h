/**
 * Memory allocations into a ram file
 * This works like Redis but not randomly accessable
 * Instead, it actually reads through sequential bytes
 */

#ifndef _sram_h
#define _sram_h

#include <SD.h>

#define MaxNameLength 32
#define HeaderSize 41 // I don't want to call sizeof everytime.

struct memoryBlockHeader {
  uint16_t pid;                // 2 bytes - process id
  char varname[MaxNameLength]; // 32 bytes
  int8_t exists;                 // 1 byte - is the variable in use?
  uint32_t position;           // 4 bytes - byte position in the file
  uint16_t size;               // 2 bytes - size of the variable
};                             // Total 41 bytes

class SRam {
private:
  File ram;
  memoryBlockHeader findVariable(char *name, uint16_t pid);
  bool isOpen;
  // TODO: Create memory defragmentation
  // void defrag();
public:
  SRam();
  ~SRam();

  void open(const char *filename);
  void close();
  void allocateVariable(char *name, uint16_t pid, uint16_t variableSize);
  void deleteVariable(char *name, uint16_t pid);

  // we might not want to read the whole variable into memory, so let's define
  // positions
  uint16_t read(char *name, uint16_t pid, uint32_t pos, char *buffer,
                uint16_t size);
  uint16_t readAll(char *name, uint16_t pid, char *buffer);
  // Write data into variable
  uint16_t write(char *name, uint16_t pid, uint32_t pos, char *data,
                 uint16_t size);
};

#endif
