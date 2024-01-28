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

#include "VGAHELPR.H"



fixed16_16 SIN_ACOS[1024];
byte *VGA = (byte *)0xA0000;     // This points to video memory
word *clock = (word *)0x046C;    // This points to the 18.2hz system clock
const int int_sign_bit = 1 << sizeof(int) * 8 - 1;


// Return 1 with the sign of input integer applied, or 0 if the input is 0
// Please replace with a purely bitop way to do this
int int_sign(int number){
  return (number > 0) - (number < 0);
}

// Build mathematic tables
void build_tables(){
  int i;
  for( i=0; i<1024; i++){
    SIN_ACOS[i]=sin(acos((float)i/1024))*0x10000L;
  }
}


//***********************************************************************
// Initializes values that need to be initialized to use all of VGAHelpr
//***********************************************************************
void vgahlpr_init(){
  build_tables();
#ifdef __DJGPP__
  if(__djgpp_nearptr_enable()==0){
    printf("Could get access to first 640K of memory.\n");
    exit(-1);
  }

  VGA   +=                 __djgpp_conventional_base;
  clock =  (void *)clock + __djgpp_conventional_base;
#endif
}


//************************************************************************************************
// Deinitializes anything initialized in vgahlpr_init() that needs to be explicitly deinitialized
//************************************************************************************************
void vgahlpr_deinit(){
#ifdef __DJGPP__
  __djgpp_nearptr_disable();
#endif
}


//****************
// Set video mode
//****************
void vgahlpr_set_mode(byte mode){
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  int86(VIDEO_INTERRUPT, &regs, &regs);
}


//**************************************************************
// Draw pixels the fast way by directly writing to video memory
// ------------------------------------------------------------
//
// All the other draw functions will rely on this function
//
//**************************************************************
void vgahlpr_draw_pixel(int x, int y, byte color){
#ifdef DEBUG
  if(x<0 && x>=320 && y<0 && y>=200){
    vgahlpr_set_mode(TEXT_MODE);
    printf("Error, pixel out of bounds!\n");
    return;
  }
#endif
  // Use bit shift multiplication to reach the line corresponding to the
  // Y-value.
  // Bit shift multiplication products are always a power of two, so we
  // add two numbers together that produce a multiplication equal to
  // Y * 320, as  Y * 320 == (Y * 256) + (Y * 64).
  VGA[ (y<<8) + (y<<6) + x ] = color;
}
//*******************************************
// Read the color from a pixel on the screen
//*******************************************
byte vgahlpr_read_pixel(int x, int y){
  return VGA[ (y<<8) + (y<<6) + x ];
}


//**********************
// Draw a vertical line
//**********************
void vgahlpr_draw_vline(int x, int y, int rise, byte color){
  // Take the sign from the length variable and apply it to 1 to use as
  // the number to decrement it to zero, regardless of the sign of the value
  // passed.
  vgahlpr_draw_pixel(x,y,color);
  int i=rise, decrementer = int_sign(rise);
  while(i){
    vgahlpr_draw_pixel(x, y+i, color);
    i-=decrementer;
  }
}
//**********************************************
// Draw a vertical line in absolute coordinates
//**********************************************
void vgahlpr_draw_vline_absolute(int x, int start_y, int end_y, byte color){
  vgahlpr_draw_vline(x, start_y, end_y-start_y, color);
}


//************************
// Draw a horizontal line
//************************
void vgahlpr_draw_hline(int x, int y, int run, byte color){
  // Take the sign from the length variable and apply it to 1 to use as
  // the number to decrement it to zero, regardless of the sign of the value
  // passed.
  vgahlpr_draw_pixel(x,y,color);
  int i=run, decrementer = int_sign(run);
  while(i){
    vgahlpr_draw_pixel(x+i, y, color);
    i-=decrementer;
  }
}
//**********************************************
// Draw a horizontal line in absolute coordinates
//**********************************************
void vgahlpr_draw_hline_absolute(int start_x, int y, int end_x, byte color){
  vgahlpr_draw_vline(start_x, y, end_x-start_x, color);
}



//******************************************************
// Draw a line using Bresenham's line-drawing algorithm
//******************************************************
void vgahlpr_draw_line(int x, int y, int run, int rise, byte
color){
  int i,
    abs_run,    abs_rise,
    run_sign,   rise_sign,
    half_run,   half_rise,
    position_x, position_y;

  abs_run    = abs(run);
  abs_rise   = abs(rise);
  run_sign   = int_sign(run);
  rise_sign  = int_sign(rise);
  half_run   = abs_run>>1;
  half_rise  = abs_rise>>1;
  position_x = x;
  position_y = y;

  VGA[(position_y<<8) + (position_y<<6) + position_x] = color;

  if(abs_run >= abs_rise){ // If line is more horizontal than vertical
    for(i=0; i<abs_run; i++){
      half_run += abs_rise;
      if (half_run>=abs_run){
        half_run -= abs_run;
        position_y+=rise_sign;
      }
      position_x += run_sign;
      vgahlpr_draw_pixel(position_x, position_y, color);
    }
  }
  else{ // If the line is more vertical than horizontal
    for(i=0; i<abs_rise; i++){
      half_rise+=abs_run;
      if(half_rise>=abs_rise){
        half_rise -= abs_rise;
        position_x += run_sign;
      }
      position_y += rise_sign;
      vgahlpr_draw_pixel(position_x, position_y, color);
    }
  }
}
//***********************************************
// Draw a Bresenham line in absolute coordinates
//***********************************************
void vgahlpr_draw_line_absolute(int start_x, int start_y, int end_x, int end_y, byte color){
  vgahlpr_draw_line(start_x, start_y, end_x-start_x, end_y-start_y, color);
}


//******************
// Draw a rectangle
//******************
void vgahlpr_draw_rectangle(int x, int y, int run, int rise, byte color){

  vgahlpr_draw_hline(x,     y,      run,  color);
  vgahlpr_draw_hline(x,     y+rise, run,  color);

  vgahlpr_draw_vline(x,     y,      rise, color);
  vgahlpr_draw_vline(x+run, y,      rise, color);
}
//******************************************
// Draw a rectangle in absolute coordinates
//******************************************
void vgahlpr_draw_rectangle_absolute(int start_x, int start_y, int end_x, int end_y, byte color){
  vgahlpr_draw_rectangle(start_x, start_y, end_x-start_x, end_y-start_y, color);
}


//***************************
// Draw a rectangle (filled)
//***************************
void vgahlpr_draw_filled_rectangle(int x, int y, int run, int rise, byte color){
  int i=run, decrementer = int_sign(run);
  vgahlpr_draw_vline(x,y,rise,color);
  while(i){
    vgahlpr_draw_vline(x+i,y,rise,color);
    i-=decrementer;
  }
}
//***************************************************
// Draw a rectangle (filled) in absolute coordinates
//***************************************************
void vgahlpr_draw_filled_rectangle_absolute(int start_x, int start_y, int end_x, int end_y, byte color){
  vgahlpr_draw_filled_rectangle(start_x, start_y, end_x-start_x, end_y-start_y, color);
}


//***************************************************************************
// Draw a rectangle with an outline in a different color from the fill color
//***************************************************************************
void vgahlpr_draw_outlined_rectangle(int x, int y, int run, int rise, byte outline_color, byte fill_color){
  int x_offset = int_sign(run), y_offset = int_sign(rise);
  vgahlpr_draw_rectangle(        x,   y,   run,   rise,   outline_color);
  vgahlpr_draw_filled_rectangle( x+x_offset, y+y_offset, run-(2*x_offset), rise-(2*y_offset), fill_color);
}
//***************************************************************************************************
// Draw a rectangle with an outline in a different color from the fill color in absolute coordinates
//***************************************************************************************************
void vgahlpr_draw_outlined_rectangle_absolute(int start_x, int start_y, int end_x, int end_y, byte outline_color, byte fill_color){
  vgahlpr_draw_outlined_rectangle(start_x, start_y, end_x-start_x, end_y-start_y, outline_color, fill_color);
}


//***********************
// Draw a circle (empty)
//***********************
void vgahlpr_draw_circle(int center_x, int center_y, int radius, byte color){
  fixed16_16 n=0,
             inverse_radius = (1/(float)radius)*0x10000L;
  int  difference_x = 0,
       difference_y = radius-1,
       i;
  word x_offset,
       y_offset,
       offset = (center_y<<8)+(center_y<<6)+center_x;
  while(difference_x<=difference_y){
    x_offset = (difference_x<<8) + (difference_x<<6);
    y_offset = (difference_y<<8) + (difference_x<<6);
    VGA[offset+i-x_offset] = color; // Octant 0
    VGA[offset+difference_x-y_offset] = color; // Octant 1
    VGA[offset-difference_x-y_offset] = color; // Octant 2
    VGA[offset-difference_y-x_offset] = color; // Octant 3
    VGA[offset-difference_y+x_offset] = color; // Octant 4
    VGA[offset-difference_x+y_offset] = color; // Octant 5
    VGA[offset+difference_x+y_offset] = color; // Octant 6
    VGA[offset+difference_y+x_offset] = color; // Octant 7
    difference_x++;
    n+=inverse_radius;
    difference_y = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
  }
}
//***********************************************
// Draw a circle (empty) in absolute coordinates
//***********************************************
void vgahlpr_draw_circle_absolute(int start_x, int start_y, int end_x, int end_y, byte color){
  int difference_x = end_x-start_x;
  int difference_y = end_y-start_y;
  // I feel like this can be optimized
  int radius = ((abs(difference_x)>abs(difference_y))?difference_y:difference_x)>>1;
  vgahlpr_draw_circle(start_x+radius,start_y+radius,radius,color);
}






