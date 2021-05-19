#include "mdisplay.h"
#include "font.h"
#include <SPI.h>
#include <VGAX.h>

#define CONSOLE_W 25
#define CONSOLE_H 10

char vga_buffer[CONSOLE_H][CONSOLE_W];
int row = 0;
char input_vga_buffer[CONSOLE_W];

int start_vga_driver()
{
    VGAX::begin(false);
    VGAX::clear(0);
    memset(vga_buffer, '\0', CONSOLE_H * CONSOLE_W);
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
        memset(vga_buffer[i - 1], '\0', CONSOLE_W);
        strcpy(vga_buffer[i - 1], vga_buffer[i]);
        memset(vga_buffer[i], '\0', CONSOLE_W);
    }
}

void push_buffer()
{
    VGAX::clear(0);
    VGAX::printSRAM((byte *)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, input_vga_buffer, 2, 2, 1);
    for (uint8_t i = 0; i < SCREEN_WIDTH+1; i++)
    {
        VGAX::putpixel(i, 7, 3);
        VGAX::putpixel(i, 0, 3);
        VGAX::putpixel(i, SCREEN_HEIGHT, 3);
    }
    for (uint8_t i = 0; i < SCREEN_HEIGHT; i++)
    {
        VGAX::putpixel(0, i, 3);
        VGAX::putpixel(SCREEN_WIDTH, i, 3);
    }
    for (uint8_t i = 0; i < CONSOLE_H; i++)
    {
        VGAX::printSRAM((byte *)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, vga_buffer[i], 2, 9 + (i * 7), 1);
    }
}

void inc_row()
{
    row++;
    if (row == CONSOLE_H)
    {
        scroll_buffer();
        row--;
    }
    memset(vga_buffer[row], 0, CONSOLE_W);
}

int println_vga(const char *text)
{
    print_vga(text);
    inc_row();
    return 0;
}

int print_vga_input(const char *text)
{
    int diff = CONSOLE_W - strlen(input_vga_buffer) + strlen(text);
    if (diff < 0)
    {
        int pos = 0;
        for (unsigned int i = diff; i < CONSOLE_W; i++)
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
        for (unsigned int i = diff; i < CONSOLE_W; i++)
        {
            input_vga_buffer[pos++] = input_vga_buffer[i];
        }
        input_vga_buffer[pos] = '\0';
    }
    if (ch == 8)
    {
        input_vga_buffer[strlen(input_vga_buffer) - 1] = '\0';
    }
    else
    {
        sprintf(input_vga_buffer, "%s%c", input_vga_buffer, ch);
    }
    push_buffer();
    return 0;
}

int println_vga(double numeric)
{
    print_vga(numeric);
    inc_row();
    return 0;
}

int print_vga(const char *text)
{
    uint8_t row_len = strlen(vga_buffer[row]);
    uint8_t counter = row_len;
    for (uint8_t i = 0; i < strlen(text); i++)
    {
        if (counter == CONSOLE_W-1)
        {
            vga_buffer[row][CONSOLE_W-1] = '\0';
            inc_row();
            counter = 0;
        }
        if (text[i] == '\n')
        {
            inc_row();
            counter = 0;
            continue;
        }
        vga_buffer[row][counter++] = text[i];
    }
    vga_buffer[row][counter] = '\0';

    return 0;
}

int print_vga(double numeric)
{
    char text[12];
    dtostrf(numeric, 7, 3, text);

    uint8_t row_len = strlen(vga_buffer[row]);
    uint8_t counter = row_len;
    for (uint8_t i = 0; i < strlen(text); i++)
    {
        if (counter == CONSOLE_W-1)
        {
            vga_buffer[row][CONSOLE_W-1] = '\0';
            inc_row();
            counter = 0;
        }
        vga_buffer[row][counter++] = text[i];
    }
    vga_buffer[row][counter] = '\0';
    return 0;
}

int clear_buffer()
{
    memset(vga_buffer, '\0', CONSOLE_W * CONSOLE_H);
    row = 0;
    push_buffer();
    return 0;
}

int clear_input_buffer()
{
    memset(input_vga_buffer, '\0', CONSOLE_W);
    push_buffer();
    return 0;
}

int pixel(byte x, byte y, byte c)
{
    VGAX::putpixel(x, y, c);
    return 0;
}

int clear_display()
{
    VGAX::clear(0);
    return 0;
}
