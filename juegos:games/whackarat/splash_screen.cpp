#include <Arduino.h>
#include <TVout.h>
#include "misc_bitmaps.h"
#include "splash_screen.h"
#include "bitmap_funcs.h"

extern TVout tv;

SplashScreen::SplashScreen() {
}

void SplashScreen::draw() {
  tv.print(0, 0, "WHACK A RAT!");
  
  if((millis()/500) % 2 == 0) {
    tv.print(0, 10, "PRESS A KEY!");  
    tv.bitmap(70, 25, treffer_bitmap);
  } else {
    tv.draw_rect(0, 10, 100, 10, 0, 0);
    tv.draw_rect(70, 25, 56, 23, 0, 0);
    tv.bitmap(72, 48, ratte_bitmap);
  }
}

