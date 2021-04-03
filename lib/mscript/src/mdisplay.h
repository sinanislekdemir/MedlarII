#ifndef _mdisplay_h
#define _mdisplay_h

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int start_oled_driver();
int println_oled(const char *text);
int println_oled(double text);
int print_oled(const char *text);
int print_oled(double text);

#endif
