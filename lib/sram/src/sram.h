/**
 * Memory allocations into a ram file
 * This works like Redis but not randomly accessable
 * Instead, it actually reads through sequential bytes
 */

#ifndef _sram_h
#define _sram_h

#include <SD.h>

#define MaxNameLength 32
#define HeaderSize 42 // I don't want to call sizeof everytime.

#define TYPE_CHAR 1
#define TYPE_BYTE 1
#define TYPE_NUM 4
#define TYPE_DOUBLE 4

struct memoryBlockHeader
{
    uint16_t pid;                // 2 bytes - process id
    char varname[MaxNameLength]; // 32 bytes
    int8_t exists;               // 1 byte - is the variable in use?
    uint32_t position;           // 4 bytes - byte position in the file
    uint16_t size;               // 2 bytes - size of the variable
    uint8_t type;                // 1 byte
};                               // Total 42 bytes

class SRam
{
private:
    File ram;
    // TODO: Create memory defragmentation
    // void defrag();
public:
    SRam();
    ~SRam();

    bool isOpen;
    memoryBlockHeader findVariable(char *name, uint16_t pid);
    void open(const char *filename);
    void close();
    void allocateVariable(char *name, uint16_t pid, uint16_t variableSize, uint8_t variable_type);
    void deleteVariable(char *name, uint16_t pid);

    // we might not want to read the whole variable into memory, so let's define
    // positions
    uint16_t read(char *name, uint16_t pid, uint32_t pos, char *buffer,
                  uint16_t size);
    uint16_t readAll(char *name, uint16_t pid, char *buffer);
    // Write data into variable
    uint16_t write(char *name, uint16_t pid, uint32_t pos, char *data,
                   uint16_t size);
    int get_var(char *text, uint16_t pid, char *back);
    void dump();
};

uint8_t argc(char *text, char delimiter);
int extract(char *text, char delimiter, uint8_t part, char *back);
int rest(char *text, uint8_t pos, char *back);
char *dtoc(double d);
double ctod(char *data);
uint8_t argc(char *text, char delimiter);

#endif
