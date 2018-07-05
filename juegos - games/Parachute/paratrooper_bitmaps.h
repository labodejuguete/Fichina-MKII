#ifndef PARATROOPER_BITMAPS_H
#define PARATROOPER_BITMAPS_H
typedef const unsigned char prog_char;
typedef const unsigned char prog_uchar;
#include <avr/pgmspace.h>

typedef struct
{
  byte x;
  byte y;
} Coord;

#define SKY_SIZE 20  // total number of slots in the sky we can fill with paratroopers (including those under the RHS tree)

extern Coord paratrooperPositions[ SKY_SIZE ];

extern prog_uchar paratrooper00_bitmap[];
extern prog_uchar paratrooper01_bitmap[];
extern prog_uchar paratrooper02_bitmap[];
extern prog_uchar paratrooper03_bitmap[];
extern prog_uchar paratrooper04_bitmap[];
extern prog_uchar paratrooper05_bitmap[];
extern prog_uchar paratrooper06_bitmap[];
extern prog_uchar paratrooper10_bitmap[];
extern prog_uchar paratrooper11_bitmap[];
extern prog_uchar paratrooper12_bitmap[];
extern prog_uchar paratrooper13_bitmap[];
extern prog_uchar paratrooper14_bitmap[];
extern prog_uchar paratrooper15_bitmap[];
extern prog_uchar paratrooper20_bitmap[];
extern prog_uchar paratrooper21_bitmap[];
extern prog_uchar paratrooper22_bitmap[];
extern prog_uchar paratrooper23_bitmap[];
extern prog_uchar paratrooper24_bitmap[];
extern prog_uchar paratrooper30_bitmap[];
extern prog_uchar paratrooper31_bitmap[];

extern prog_uchar *paratrooper_bitmaps[ SKY_SIZE ]; 

#define NUM_VICTIMS 6  // number of frames in the shark attack sequence

extern prog_uchar victim1_bitmap[];
extern prog_uchar victim2_bitmap[];
extern prog_uchar victim3_bitmap[];
extern prog_uchar victim4_bitmap[];
extern prog_uchar victim5_bitmap[];
extern prog_uchar victim6_bitmap[];

extern prog_uchar *victim_bitmaps[ NUM_VICTIMS ]; 
extern Coord victim_positions[ NUM_VICTIMS ];

#endif PARATROOPER_BITMAPS_H



