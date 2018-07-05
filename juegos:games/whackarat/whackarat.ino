/*
 * Simon Game for HackVision.
 */
#include <TVout.h>
#include <fontALL.h>
#include <Controllers.h>
#include "misc_bitmaps.h"
#include "splash_screen.h"
#include "game.h"
#include "highscore.h"

#define W 128
#define H 96

TVout tv;

SplashScreen splash;
Game game;
Highscore highscore;

#define  SPLASH     0
#define  GAME       1
#define  HIGHSCORE  2

byte spielmodus = SPLASH;

void setup()  {
  // If pin 12 is pulled LOW, then the PAL jumper is shorted.
  pinMode(12, INPUT);
  digitalWrite(12, HIGH);

  if (digitalRead(12) == LOW) {
    tv.begin(_PAL, W, H);
  } 
  else {
    tv.begin(_NTSC, W, H);
  }

  randomSeed(analogRead(0));

  tv.select_font(font8x8);
}

void loop() {
  // malen...
  switch (spielmodus) {
  case SPLASH:
    splash.draw();
    break;
  case GAME:
    game.calcndraw();
    if(game.gameOver()) {
      spielmodus = HIGHSCORE;
      highscore.add(game.getPoints());
    }
    break;
  case HIGHSCORE:
    highscore.draw();
    if(highscore.ready()) {
      game.waitUntilKeyIsPressed();
      game.waitUntilNoKeyIsPressed();
      tv.clear_screen();
      spielmodus = SPLASH;
    }
    break;
  }
  
  // Tasten...
  switch (spielmodus) {
  case SPLASH:
    if(Controller.leftPressed() || Controller.rightPressed() || Controller.upPressed() || Controller.downPressed() || Controller.firePressed()) {
      game.waitUntilNoKeyIsPressed();
      game.initNewGame();
      spielmodus = GAME;
    }
    break;
  case GAME:
    break;
  case HIGHSCORE:
    break;
  }  
}


