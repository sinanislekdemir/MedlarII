#ifndef _mscript_h
#define _mscript_h

#include <sram.h>
#include "statements.h"
#include "freemem.h"
#include "const.h"


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

class MScript
{
private:
    bool isOpen;
    File file;
    int prepare();
    int preParse();
    int read_memory();
    int get_line_length();
    // call call_stack[32];             // Max allowed nested jumps
    SRam memory;
    uint16_t pid;
    unsigned long sleep_duration;
    unsigned long sleep_start;
    void reset_buffer();
    bool get_var(char *text, char *back);
    bool s_input;
    int input_pos;
    char *input_var;

public:
    bool finished;
    MScript();
    ~MScript();
    int step();
    int exec(char *cmd);
    int open(uint16_t pid, char *filename);
    int close();
};

void setup_statements();
#endif
