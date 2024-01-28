/*
  +=========================+
  |                         |
  | VGA's Little Helper Lib |
  |                         |
  +=========================+

  Created by: Christopher DeBoy

  Based largely on code from David Brackeen's
  256-Color VGA programming in C series of tutorials

  Licensed as Public domain where available,
  otherwise, use the Unlicense license.

*/

#ifndef VGA_HELPER_H
#define VGA_HELPER_H


#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>
#include <sys/nearptr.h>

#define VIDEO_INTERRUPT    0x10  // Bios video Interrupt
#define WRITE_DOT          0x0C  // BIOS function to plot pixel
#define SET_MODE           0x00  // BIOS function to set the video mode
#define VGA_256_COLOR_MODE 0x13  // 256 color mode
#define TEXT_MODE          0x03  // 80x25 text mode

#define SCREEN_WIDTH       320   // Screen width in mode 13h
#define SCREEN_HEIGHT      200   // Screen height in mode 13h
#define NUMBER_COLORS      256   // Number of colors in mode 13h


typedef unsigned char  byte;
typedef unsigned short word;
typedef long           fixed16_16;

extern fixed16_16 SIN_ACOS[1024];
extern byte *VGA;     // This points to video memory
extern word *clock;   // This points to the 18.2hz system clock
extern const int int_sign_bit;

// Returns the sign of input number
static inline int int_sgn(int number);
void build_tables();

// Initializes values that need to be initialized to use all of VGAHelpr
void vgahlpr_init();

// Deinitializes anything initialized in vgahlpr_init() that needs to be explicitly deinitialized
void vgahlpr_deinit();

// Set the video mode
void vgahlpr_set_mode(byte mode);

// Draw pixels the fast way by directly writing to video memory
static inline void vgahlpr_draw_pixel(int x, int y, byte color);
// Read the color of a pixel from the screen
byte vgahlpr_read_pixel(int x, int y);

// Draw a vertical line
inline void vgahlpr_draw_vline(int x, int y, int length, byte color);
// Draw a vertical line in absolute coordinates
inline void vgahlpr_draw_vline_absolute(int x, int start_y, int end_y, byte color);

// Draw a horizontal line
inline void vgahlpr_draw_hline(int x, int y, int length, byte color);
// Draw a horizontal line in absolute coordinates
inline void vgahlpr_draw_hline_absolute(int start_x, int y, int end_x, byte color);

// Draw a Bresenham line
inline void vgahlpr_draw_line(int x, int y, int run, int rise, byte color);
// Draw a Bresenham line in absolute coordinates
inline void vgahlpr_draw_line_absolute(int start_x, int start_y, int end_x, int end_y, byte color);

// Draw a rectangle (empty)
void vgahlpr_draw_rectangle(int x, int y, int run, int rise, byte color);
// Draw a rectangle (empty) in absolute coordinates
void vgahlpr_draw_rectangle_absolute(int start_x, int start_y, int end_x, int end_y, byte color);

// Draw a rectangle (filled)
inline void vgahlpr_draw_filled_rectangle(int x, int y, int run, int rise, byte color);
// Draw a rectangle (filled) in absolute coordinates
inline void vgahlpr_draw_filled_rectangle_absolute(int start_x, int start_y, int end_x, int end_y, byte color);

// Draw a rectangle with an outline in a different color from the fill color
void vgahlpr_draw_outlined_rectangle(int x, int y, int run, int rise, byte outline_color, byte fill_color);
// Draw a rectangle with an outline in a different color from the fill color in absolute coordinates
void vgahlpr_draw_outlined_rectangle_absolute(int start_x, int start_y, int end_x, int end_y, byte outline_color, byte fill_color);

// Draw a circle (empty)
void vgahlpr_draw_circle(int center_x, int center_y, int radius, byte color);
// Draw a circle (empty) in absolute coordinates
void vgahlpr_draw_circle_absolute(int start_x, int start_y, int end_x, int end_y, byte color);

// Draw a circle (filled)
void vgahlpr_draw_filled_circle(int center_x, int center_y, int radius, byte color);

// Draw an elipse (empty)
void vgahlpr_draw_elipse(int center_x, int center_y, int radius_x, int radius_y, byte color);

// Draw an elipse (filled)
void vgahlpr_draw_filled_elipse(int center_x, int center_y, int radius_x, int radius_y, byte color);



#endif
