#ifndef _mscript_h
#define _mscript_h

#include "const.h"
#include "freemem.h"
#include "statements.h"
#include <sram.h>

// Statement definition
struct statement
{
    char command[16];            // statement keyword like print
    int (*function)(context *c); // context that might be needed
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
