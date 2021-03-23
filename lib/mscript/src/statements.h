#ifndef _statements_h
#define _statements_h
#include <SD.h>
#include <sram.h>

#define MaxLineLength 120
// Function context to make a function work.
struct context
{
    uint16_t pid;
    SRam *memory;
    File *script;
    char *buffer;
};

int m_sprint(context *c);
int m_goto(context *c);
int m_inc(context *c);
int m_jump(context *c);

#endif
