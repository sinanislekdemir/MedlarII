#ifndef _statements_h
#define _statements_h

#include "const.h"
#include "mdisplay.h"
#include <SD.h>
#include <sram.h>

// Function context to make a function work.
struct context
{
    uint16_t pid;
    SRam *memory;
    File *script;
    char *buffer;
    char *back;
};

void b();
int m_print(context *c);
int m_fopen(context *c);
int m_fread(context *c);
int m_sread(context *c);
int m_fwrite(context *c);
int m_fsize(context *c);

int m_inc(context *c);
int m_jump(context *c);
int m_equals(context *c);
int m_math(context *c);
int m_digitalwrite(context *c);
int m_delay(context *c);
int m_pinmode(context *c);
int m_digitalread(context *c);
int m_analogread(context *c);
int m_analogwrite(context *c);
int m_clear(context *c);
int m_pixel(context *c);
int math_command(context *c, double *numbers);
#endif
