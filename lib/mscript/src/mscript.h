#ifndef _mscript_h
#define _mscript_h

#include <sram.h>
#include "statements.h"

#define MaxLineLength 120
#define MapCacheSize 32
#define M_MEMORY ".memory"
#define M_CODE ".code"
#define M_AUTHOR "@author"
#define M_APPNAME "@appname"
#define M_LOAD "@load"
#define STATEMENT_COUNT 13

// Program meta that describes the script
struct meta
{
    char author[32];
    char appname[32];
    uint8_t mversion;
}; // 65 byte

// Keep a cache of labels and positions
struct labelMap
{
    char filename[64];
    char label[16];
    uint32_t byte_position;
}; // 84 byte



// Statement definition
struct statement
{
    char command[16];           // statement keyword like print
    int (*function)(context *c); // context that might be needed
};

// jump record to keep track of calls
struct call
{
    uint32_t from_byte;
    uint32_t to_byte;
    bool free;
};

// HERE ARE THE DEFS

class MScript
{
private:
    bool isOpen;
    char buffer[MaxLineLength];
    File file;
    int prepare();
    int preParse();
    int read_memory();
    int read_meta();
    call call_stack[32];             // Max allowed nested jumps
    labelMap mapping[MapCacheSize]; // This is a quick reach cache for the first N labels
    SRam memory;
    statement statements[STATEMENT_COUNT];
    uint16_t pid;
    void reset_buffer();
    bool get_var(char *text, char *back);

public:
    bool finished;
    meta scriptMeta;
    MScript();
    ~MScript();
    int step();
    int exec();
    int open(uint16_t pid, char *filename);
    int close();
};

#endif
