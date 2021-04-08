#include "mdisplay.h"
#include "font.h"
#include <SPI.h>
#include <VGAX.h>

#define CONSOLE_W 23
#define CONSOLE_H 11
#define CONSOLE_C 253

int cursor_x = 0;
int cursor_y = 0;

char vga_buffer[CONSOLE_C];

int start_vga_driver()
{
    VGAX::begin(false);
    VGAX::clear(0);
    return 0;
}

unsigned int f_millis()
{
    return VGAX::millis();
}

unsigned long f_micros()
{
    return VGAX::micros();
}

void f_delay(int ms)
{
    VGAX::delay(ms);
}

int real_len(const char *c)
{
    int len = 0;
    for (unsigned int i = 0; i < strlen(c); i++)
    {
        if (c[i] == '\n')
        {
            len += CONSOLE_W - (i % CONSOLE_W);
        }
        else
        {
            len += 1;
        }
    }
    return len;
}

char *wrap_buffer(const char *incoming)
{
    char *res = (char *)malloc(strlen(incoming) + 32);
    memset(res, 0, strlen(incoming) + 32);
    int line_length = 0;
    int pos = 0;
    for (uint8_t i = 0; i < strlen(incoming); i++)
    {
        line_length++;
        if (incoming[i] == '\n')
        {
            line_length = 0;
            res[pos++] = '\n';
            continue;
        }
        if (line_length == CONSOLE_W)
        {
            res[pos++] = incoming[i];
            res[pos++] = '\n';
            line_length = 0;
            continue;
        }
        res[pos++] = incoming[i];
    }
    res[pos] = '\0';
    return res;
}

void scroll_buffer(char *buffer, int width, int height)
{
    uint8_t pos = 0;
    uint8_t slen = strlen(buffer);

    char *new_buffer = (char *)malloc(slen);
    memset(new_buffer, '\0', slen);

    bool first_line = true;
    for (uint8_t i = 0; i < slen; i++)
    {
        if (!first_line)
        {
            new_buffer[pos++] = buffer[i];
        }
        if (i == width || buffer[i] == '\n')
        {
            first_line = false;
        }
    }
    new_buffer[pos] = '\0';
    strcpy(buffer, new_buffer);
    free(new_buffer);
}

void push_buffer()
{
    bool a = false;
    for (uint8_t i = 0; i < CONSOLE_C; i++)
    {
        if (a)
        {
            vga_buffer[i] = '\0';
        }
        if (vga_buffer[i] == '\0')
        {
            a = true;
        }
    }
    VGAX::clear(0);
    VGAX::printSRAM((byte *)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, vga_buffer, 2, 2, 1);
}

int println_vga(const char *text)
{
    char *ntext = wrap_buffer(text);
    int buffer_len = real_len(vga_buffer);
    int text_len = real_len(ntext);
    int total_len = buffer_len + text_len;

    if (total_len > CONSOLE_C)
    {
        while (total_len > CONSOLE_C)
        {
            scroll_buffer(vga_buffer, CONSOLE_W, CONSOLE_H);
            total_len = real_len(vga_buffer) + text_len;
        }
    }

    sprintf(vga_buffer, "%s%s\n", vga_buffer, ntext);
    free(ntext);
    push_buffer();
    return 0;
}

int println_vga(double text)
{
    char *nn = (char *)malloc(16);
    memset(nn, 0, 16);
    sprintf(nn, "%f", text);

    int buffer_len = real_len(vga_buffer);
    int text_len = real_len(nn);
    int total_len = buffer_len + text_len;
    free(nn);
    if (total_len > CONSOLE_C)
    {
        while (total_len > CONSOLE_C)
        {
            scroll_buffer(vga_buffer, CONSOLE_W, CONSOLE_H);
            total_len = real_len(vga_buffer) + text_len;
        }
    }
    sprintf(vga_buffer, "%s%s\n", vga_buffer, nn);
    push_buffer();
    return 0;
}

int print_vga(const char *text)
{
    char *ntext = wrap_buffer(text);
    int buffer_len = real_len(vga_buffer);
    int text_len = real_len(ntext);
    int total_len = buffer_len + text_len;

    if (total_len > CONSOLE_C)
    {
        while (total_len > CONSOLE_C)
        {
            scroll_buffer(vga_buffer, CONSOLE_W, CONSOLE_H);
            total_len = real_len(vga_buffer) + text_len;
        }
    }
    sprintf(vga_buffer, "%s%s", vga_buffer, ntext);
    free(ntext);
    push_buffer();
    return 0;
}

int print_vga(double text)
{
    char *nn = (char *)malloc(16);
    memset(nn, 0, 16);
    sprintf(nn, "%f", text);

    int buffer_len = real_len(vga_buffer);
    int text_len = real_len(nn);
    int total_len = buffer_len + text_len;
    free(nn);
    if (total_len > CONSOLE_C)
    {
        while (total_len > CONSOLE_C)
        {
            scroll_buffer(vga_buffer, CONSOLE_W, CONSOLE_H);
            total_len = real_len(vga_buffer) + text_len;
        }
    }
    sprintf(vga_buffer, "%s%s", vga_buffer, nn);
    push_buffer();
    return 0;
}
