#ifndef __BITMAP_FUNCS__
#define __BITMAP_FUNCS__
typedef const unsigned char prog_char;
typedef const unsigned char prog_uchar;
void overlaybitmap(uint8_t x, uint8_t y, const unsigned char * bmp );
void erasebitmap(uint8_t x, uint8_t y, const unsigned char * bmp );
void eraseBitmapRect( uint8_t x, uint8_t y, const unsigned char * bmp );

#endif __BITMAP_FUNCS__
