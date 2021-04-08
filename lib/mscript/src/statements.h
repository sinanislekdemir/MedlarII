#ifndef _statements_h
#define _statements_h

#include <SD.h>
#include <sram.h>
#include "const.h"
#include "mdisplay.h"


// Function context to make a function work.
struct context
{
    uint16_t pid;
    SRam *memory;
    File *script;
    char *buffer;
};

void b();
int m_sprint(context *c);
int m_sprintln(context *c);
int m_oprint(context *c);
int m_oprintln(context *c);
int m_fopen(context *c);
int m_fread(context *c);
int m_fwrite(context *c);
int m_fsize(context *c);

int m_inc(context *c);
int m_jump(context *c);
int m_equals(context *c);
int m_add(context *c);
int m_sub(context *c);
int m_div(context *c);
int m_digitalwrite(context *c);
int m_mul(context *c);
int m_delay(context *c);
int m_pinmode(context *c);
int m_digitalread(context *c);
int m_analogread(context *c);
int m_analogwrite(context *c);
int math_command(context *c, double *numbers);
#endif
