#ifndef MISC_BITMAPS_H
#define MISC_BITMAPS_H
typedef const unsigned char prog_char;
typedef const unsigned char prog_uchar;
#include <avr/pgmspace.h>

// positions of the heli's rotor blades
#define heliRotorLeftX1 91
#define heliRotorLeftX2 108
#define heliRotorLeftY 1
#define heliRotorRightX1 106
#define heliRotorRightX2 123
#define heliRotorRightY 3

extern prog_uchar heli_bitmap[];  // 32 × 17 pixels
extern prog_uchar water_bitmap[]; // 128 × 22 pixels
extern prog_uchar left_bitmap[];  // 39 × 52 pixels
extern prog_uchar right_bitmap[]; // 41 × 47 pixels

#endif BOAT_BITMAPS_H

