#include "mdisplay.h"
#include "font.h"
#include <SPI.h>
#include <VGAX.h>

#define CONSOLE_W 25
#define CONSOLE_H 10
#define CONSOLE_C 250

int cursor_x = 0;
int cursor_y = 0;

char vga_buffer[CONSOLE_H][CONSOLE_W];
int row = 0;
char input_vga_buffer[CONSOLE_W];

int start_vga_driver()
{
    VGAX::begin(false);
    VGAX::clear(0);
    memset(vga_buffer, '\0', CONSOLE_H*CONSOLE_W);
    memset(input_vga_buffer, '\0', CONSOLE_W);
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

void scroll_buffer()
{
    for (uint8_t i = 1; i < CONSOLE_H; i++)
    {
        memset(vga_buffer[i-1], '\0', CONSOLE_W);
        strcpy(vga_buffer[i-1], vga_buffer[i]);
        memset(vga_buffer[i], '\0', CONSOLE_W);
    }
}

void push_buffer()
{
    VGAX::clear(0);
    VGAX::printSRAM((byte *)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, input_vga_buffer, 2, 2, 1);
    for (uint8_t i = 0; i < SCREEN_WIDTH; i++)
    {
        VGAX::putpixel(i, 7, 3);
    }
    for (uint8_t i = 0; i < CONSOLE_H; i++)
    {
        VGAX::printSRAM((byte *)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, vga_buffer[i], 2, 9 + (i*7), 1);
    }
}

int println_vga(const char *text)
{
    memset(vga_buffer[row], '\0', CONSOLE_W);
    strcpy(vga_buffer[row], text);
    row++;
    if (row == CONSOLE_H)
    {
        scroll_buffer();
        row --;
    }
    push_buffer();
    return 0;
}

int print_vga_input(const char *text)
{
    int diff = CONSOLE_W - strlen(input_vga_buffer) + strlen(text);
    if (diff < 0)
    {
        int pos = 0;
        for(unsigned int i = diff; i < CONSOLE_W; i++)
        {
            input_vga_buffer[pos++] = input_vga_buffer[i];
        }
        input_vga_buffer[pos] = '\0';
    } 
    sprintf(input_vga_buffer, "%s%s", input_vga_buffer, text);
    push_buffer();
    return 0;
}

int print_vga_input(const char ch)
{
    int diff = CONSOLE_W - strlen(input_vga_buffer) + 1;
    if (diff < 0)
    {
        int pos = 0;
        for(unsigned int i = diff; i < CONSOLE_W; i++)
        {
            input_vga_buffer[pos++] = input_vga_buffer[i];
        }
        input_vga_buffer[pos] = '\0';
    }
    if (ch == 8)
    {
        input_vga_buffer[strlen(input_vga_buffer)-1] = '\0';
    } else {
        sprintf(input_vga_buffer, "%s%c", input_vga_buffer, ch);
    }
    push_buffer();
    return 0;
}

int println_vga(double numeric)
{
    memset(vga_buffer[row], '\0', CONSOLE_W);
    char s[12];
    dtostrf(numeric, 7, 3, s);
    sprintf(vga_buffer[row], "%s", s);
    row++;
    if (row == CONSOLE_H)
    {
        scroll_buffer();
        row --;
    }
    push_buffer();
    return 0;
}

int print_vga(const char *text)
{
    sprintf(vga_buffer[row], "%s%s", vga_buffer[row], text);
    push_buffer();
    return 0;
}

int print_vga(double numeric)
{
    char s[12];
    dtostrf(numeric, 7, 3, s);
    sprintf(vga_buffer[row], "%s%s", vga_buffer[row], s);
    push_buffer();
    return 0;
}

int clear_buffer()
{
    memset(vga_buffer, '\0', CONSOLE_W*CONSOLE_H);
    push_buffer();
    return 0;
}

int clear_input_buffer()
{
    memset(input_vga_buffer, '\0', CONSOLE_W);
    push_buffer();
    return 0;
}
