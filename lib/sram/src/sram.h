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
#define TYPE_FILE 5
#define MAX_FILE_PATH 128

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
    // Registers are in-memory fast access numbers suitable for fast processing of stuff.
    // it's a quick hack where file-based ram is slow.
    double registers[16];
    uint32_t get_start(char *text, uint16_t pid);
    uint32_t get_end(char *text, uint16_t pid);
    char *filename;
    int ensureOpen();
    // TODO: Create memory defragmentation
    // void defrag();
public:
    SRam();
    ~SRam();

    bool isOpen;
    memoryBlockHeader *find_variable(char *name, uint16_t pid);
    void open(const char *filename);
    void close();
    void init_fast_mem(uint16_t size);
    void allocate_variable(char *name, uint16_t pid, uint16_t variableSize, uint8_t variable_type);
    void delete_variable(char *name, uint16_t pid);

    // we might not want to read the whole variable into memory, so let's define
    // positions
    uint16_t read(char *name, uint16_t pid, uint32_t pos, char *buffer,
                  uint16_t size, bool raw);
    uint16_t read_all(char *name, uint16_t pid, char *buffer, bool raw);
    // Write data into variable
    uint16_t write(char *name, uint16_t pid, uint32_t pos, char *data,
                   uint16_t size, bool raw);
    int get_var_size(char *text, uint16_t pid);
    int get_var(char *text, uint16_t pid, char *back);
    File get_file(memoryBlockHeader *m);
    void dump();
};

uint8_t argc(char *text, char delimiter);
int extract(char *text, char delimiter, uint8_t part, char *back);
int extract_size(char *text, char delimiter, uint8_t part);
int rest(char *text, uint8_t pos, char *back);
char *dtoc(double d);
double ctod(char *data);
uint8_t argc(char *text, char delimiter);

#endif
