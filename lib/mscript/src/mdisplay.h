#ifndef _mdisplay_h
#define _mdisplay_h
#include <Arduino.h>

#define SCREEN_WIDTH 120    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels

int start_vga_driver();
int println_vga(const char *text);
int println_vga(double numeric);
int print_vga(const char *text);
int print_vga(double numeric);
int print_vga_input(const char *text);
int print_vga_input(const char ch);
int pixel(byte x, byte y, byte c);
int clear_display();
int clear_buffer();
int clear_input_buffer();

unsigned int f_millis();
unsigned long f_micros();
void f_delay(int ms);

#endif
