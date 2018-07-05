#include <TVout.h>
#include <fontALL.h>
#include <Controllers.h>
#include <EEPROM.h>
#include "paratrooper_bitmaps.h"
#include "boat_bitmaps.h"
#include "shark_bitmaps.h"
#include "bitmap_funcs.h"
#include "misc_bitmaps.h"

// TVout setup bits
TVout TV;
#define TVwidth 128
#define TVheight 96
boolean palJumper;
#define PALpin 12

// game loop timer
#define UPDATE_INTERVAL 225
unsigned long nextUpdate;

#define GAME_A_HISCORE  10  // 2 bytes for each
#define GAME_B_HISCORE  12


typedef enum
{
  eEmpty, eOccupied, eSaved
} SkyContentValue;

enum
{
  eGameA, eGameB
};

// indexes into the skyValues array
#define TOP_COL_1 0
#define BOT_COL_1 6
#define TOP_COL_2 7
#define BOT_COL_2 12
#define TOP_COL_3 13
#define BOT_COL_3 17
#define ABOVE_TREE 14
#define NEAR_TREE 15
#define BELOW_TREE 16
#define TREE_1 18
#define TREE_2 19

prog_char parachuteStr[] PROGMEM = "PARACHUTE";
prog_char wideScreenStr[] PROGMEM = "WIDE SCREEN";
prog_char gameAStr[] PROGMEM = "GAME A";
prog_char gameBStr[] PROGMEM = "GAME B";

SkyContentValue skyValues[ SKY_SIZE ];  // 0-6 = column 1, 7-12 = column 2, 13-17 = column 3, 18-19 = column 4 (in tree)
SkyContentValue *boatPos[ NUM_BOATS ] = { &(skyValues[ BOT_COL_1 ]), &(skyValues[ BOT_COL_2 ]), &(skyValues[ BOT_COL_3 ])}; // used to check for drops and catches
byte columnLoopIndexes[ 4 ] = { TOP_COL_1, TOP_COL_2, TOP_COL_3, BOT_COL_3 + 1 }; // used to iterate down each column

int score;  // current score
byte boatPosition;  // 0-2 for position on screen
byte misses; // current number of misses
byte maxParatroopers; // max allowable paratroopers based on level
byte currentParatroopers; // count of paratroopers in the sky
byte currentSharkPos;  // roaming shark
byte currentColumn; // which column is currently moving
byte sharkTimer; //controls the roaming shark animation
boolean gameActive = false; // are we playing a game?
boolean menuIsActive = true; // menu is displayed
byte selectedGame;
int gameAHiscore;
int gameBHiscore;


unsigned long loopCount = 0;


void setup()
{  
  // If pin 12 is pulled LOW, then the PAL jumper is shorted.
  pinMode( PALpin, INPUT );
  digitalWrite( PALpin, HIGH );
  palJumper = digitalRead( PALpin );
  
  TV.begin( palJumper ? NTSC : PAL, TVwidth, TVheight );
  
  
  randomSeed( analogRead( 0 ) );
  displayMenu();
}

int readEEPROM_Int( int address )
{
  byte hi, lo;
  int int_value;

  hi = EEPROM.read( address );
  lo = EEPROM.read( address + 1);
  if ((hi == 0xFF) && (lo == 0xFF)) {
    int_value = 0;
  } else {
    int_value = (hi << 8) | lo;
  }
  
  return int_value;
}

void readHiscores()
{
  gameAHiscore = readEEPROM_Int( GAME_A_HISCORE );
  gameBHiscore = readEEPROM_Int( GAME_B_HISCORE );
}

// draw all the static bits
void initPlayingArea()
{
  TV.fill( BLACK );
  // water, heli, left and right trees
  TV.bitmap( 93, 3, heli_bitmap );
  TV.bitmap( 0, 75, water_bitmap );
  TV.bitmap( 0, 22, left_bitmap );
  TV.bitmap( 93, 27, right_bitmap );
  TV.set_cursor( 0, 122 );
  TV.select_font( font4x6 );
  if ( selectedGame == eGameA )
    TV.printPGM( 0, 90, gameAStr );
  else
    TV.printPGM( 0, 90, gameBStr );
}

// draws the menu and enables input
void displayMenu()
{
  char buf[ 4 ];

  TV.fill( BLACK );
  TV.select_font( font8x8 );
  TV.draw_rect( 4, 4, 120, 87, WHITE, BLACK );
  // lop the corners off for a rounded rect
  TV.draw_rect( 4, 4, 1, 1, BLACK, BLACK );
  TV.draw_rect( 4, 90, 1, 1, BLACK, BLACK );
  TV.draw_rect( 123, 4, 1, 1, BLACK, BLACK );
  TV.draw_rect( 123, 90, 1, 1, BLACK, BLACK );
  TV.set_pixel( 5, 5, WHITE );
  TV.set_pixel( 5, 90, WHITE );
  TV.set_pixel( 123, 5, WHITE );
  TV.set_pixel( 123, 90, WHITE );
  TV.printPGM( 28, 0, parachuteStr );
  TV.printPGM( 20, 87, wideScreenStr );
  TV.draw_rect( 34, 35, 60, 10, WHITE, BLACK );
  TV.draw_rect( 34, 45, 60, 10, WHITE, BLACK );
  TV.printPGM( 40, 36, gameAStr );
  TV.printPGM( 40, 46, gameBStr );
  TV.set_pixel( 108, 91, BLACK );
  
  readHiscores();
  BufferPrint( buf, gameAHiscore, 100, 4 );
  TV.print( 97, 36, buf );
  BufferPrint( buf, gameBHiscore, 100, 4 );
  TV.print( 97, 46, buf );
  
  updateMenu( eGameA );
  menuIsActive = true;
}

// initialise everything to start a new game
void gameStart()
{
  initPlayingArea();
  score = misses = currentParatroopers = 0; // resets score, misses & jumpers on screen
  maxParatroopers = 1;  // increments based on score
  memset( skyValues, eEmpty, SKY_SIZE * sizeof( SkyContentValue ) ); // no jumpers yet
  currentColumn = random( 0, 2 ); // jumpers start in columns 0 or 1
  initBoat();
  initHeli();
  initRoamingShark();
  displayScore();
  nextUpdate = TV.millis(); // update right away
  gameActive = true;
  menuIsActive = false;
}

void initBoat()
{
  boatPosition = 1;  // centre
  drawBoat( boatPosition, true );
}

// draw or erase a single paratrooper
void drawParatrooper( byte ndx, boolean draw )
{
  if ( draw )
    overlaybitmap( paratrooperPositions[ ndx ].x, paratrooperPositions[ ndx ].y, paratrooper_bitmaps[ ndx ] );
  else
    erasebitmap( paratrooperPositions[ ndx ].x, paratrooperPositions[ ndx ].y, paratrooper_bitmaps[ ndx ] );
 }

// draw or erase a single victim
void drawVictim( byte ndx, boolean draw )
{
  if ( draw )
    overlaybitmap( victim_positions[ ndx ].x, victim_positions[ ndx ].y, victim_bitmaps[ ndx ] );
  else
    erasebitmap( victim_positions[ ndx ].x, victim_positions[ ndx ].y, victim_bitmaps[ ndx ] );
}

void drawShark( byte ndx, boolean draw )
{
  if ( draw )
    overlaybitmap( shark_positions[ ndx ].x, shark_positions[ ndx ].y, shark_bitmaps[ ndx ] );
  else
    erasebitmap( shark_positions[ ndx ].x, shark_positions[ ndx ].y, shark_bitmaps[ ndx ] );

}

// for testing only
void drawFullSky()
{
//  for(;;)
  for ( byte ndx = 0; ndx < SKY_SIZE; ndx++ )
  {
    TV.draw_rect(paratrooperPositions[ ndx ].x, paratrooperPositions[ ndx ].y, pgm_read_byte((uint32_t)(paratrooper_bitmaps[ ndx ])), pgm_read_byte((uint32_t)(paratrooper_bitmaps[ ndx ])+1), WHITE, BLACK);
//    drawParatrooper( ndx, true );
//    TV.delay(  200 );
//    drawParatrooper( ndx, false );
  }
  for ( byte ndx = 0; ndx < NUM_SHARKS; ndx++ )
    drawShark( ndx, true );
  for ( byte ndx = 0; ndx < NUM_VICTIMS; ndx++ )
    drawVictim( ndx, true );
}

// update boat position
void moveBoat( boolean dir )
{
  byte oldBoatPos = boatPosition;
  // left
  if ( !dir )
  {
    if ( boatPosition > 0 )
      boatPosition--;
  }
  else
  {
    if ( boatPosition < 2 )
      boatPosition++;
  }
  
  if ( oldBoatPos != boatPosition )
  {
    drawBoat( oldBoatPos, false );
    drawBoat( boatPosition, true );
  }
}

// draw or erase the boat
void drawBoat( byte pos, boolean draw )
{
  if ( draw )
    overlaybitmap( boatX[ pos ], BOAT_Y, boat_bitmaps[ pos ] );
  else
    erasebitmap( boatX[ pos ], BOAT_Y, boat_bitmaps[ pos ] );
}

// draw all the visible paratroopers
void drawParatroopers( byte column )  // currently ignored
{
  for ( byte ndx = 0; ndx < SKY_SIZE; ndx++ )
    if ( skyValues[ ndx ] == eEmpty )
      drawParatrooper( ndx, false );

  for ( byte ndx = 0; ndx < SKY_SIZE; ndx++ )
    if ( skyValues[ ndx ] != eEmpty )
      drawParatrooper( ndx, true );
}

// start the heli spinning, one blade at a time
void initHeli()
{
  TV.draw_line( heliRotorLeftX1, heliRotorLeftY, heliRotorLeftX2, heliRotorLeftY, INVERT );
  TV.set_pixel( heliRotorLeftX1 + ((heliRotorLeftX2 - heliRotorLeftX1)/2), heliRotorLeftY, INVERT ) ;
  TV.set_pixel( heliRotorRightX1 + ((heliRotorRightX2 - heliRotorRightX1)/2), heliRotorRightY, INVERT ) ;
}

// simply inverts the two blades
void animateHeli()
{
  TV.draw_line( heliRotorLeftX1, heliRotorLeftY, heliRotorLeftX2, heliRotorLeftY, INVERT );
  TV.draw_line( heliRotorRightX1, heliRotorRightY, heliRotorRightX2, heliRotorRightY, INVERT );
}

void loop()
{
  byte prevColumn;

  if ( gameActive && TV.millis() > nextUpdate )
  {
    if ( currentColumn == 0 )
      animateRoamingShark();

    animateHeli();
    removeSavedParatroopers();
    
    if ( !checkForDroppedParatroopers() )
    {
      checkForSavedParatroopers();
      moveParatroopers();
      createParatrooper();
      checkForSavedParatroopers();  // do this twice as we might have just moved a trooper down and into the boat
      prevColumn = currentColumn;
      currentColumn = ( currentColumn + 1 ) % 3;
    }
    else
      incrementMisses();
    
    drawParatroopers( prevColumn );
    
    checkResetMisses();
    
    if ( !gameActive )
      gameOver();
    else
      nextUpdate = TV.millis() + UPDATE_INTERVAL;
  }
  
  if ((loopCount++ % 300) == 0) {
    getInput();
  }

}

// stuff the new hiscore into EEPROM
void writeHiscore()
{
  byte hi, lo;
  int int_value;
  int address;
  
  if ( selectedGame == eGameA )
  {
    address = GAME_A_HISCORE;
    gameAHiscore = score;
  }
  else
  {
    address = GAME_B_HISCORE;
    gameBHiscore = score;
  }

  EEPROM.write( address, ((score >> 8) & 0xFF));
  EEPROM.write( address + 1, (score & 0xFF));
}

// signal game over... duh.
void gameOver()
{
  // record hiscore
  if ( ( selectedGame == eGameA && score > gameAHiscore ) || ( selectedGame == eGameB && score > gameBHiscore ) )
    writeHiscore();
  
  TV.select_font( font6x8 );
  TV.print( 37, 40, "game over" );
  TV.print( 34, 50, "press fire" );
}

// oops, we dropped one in the drink
void incrementMisses()
{
  overlaybitmap( misses_positions[ misses ], MISSES_Y, missshark );
  misses++;
  if ( misses >= 3 )
    gameActive = false;
}

// basic menu operation
void updateMenu( byte game )
{
  selectedGame = game;
  
  switch ( selectedGame )
  {
    case eGameA:
      TV.print( 25, 36, ">" );
      TV.print( 25, 46, " " );
    break;
    
    case eGameB:
      TV.print( 25, 36, " " );
      TV.print( 25, 46, ">" );
    break;
  }
}

// handles menu screen and game screen activity
boolean getInput()
{
  if ( gameActive && Controller.leftPressed() )
    moveBoat( false );
  else if ( gameActive && Controller.rightPressed() )
    moveBoat( true );
  else if ( menuIsActive && Controller.upPressed() && selectedGame == eGameB )
    updateMenu( eGameA );
  else if ( menuIsActive && Controller.downPressed() && selectedGame == eGameA )
    updateMenu( eGameB );
  else if ( menuIsActive && Controller.firePressed() )
    gameStart();
  else if ( !menuIsActive && !gameActive && Controller.firePressed() )
    displayMenu();
 }

// check if we've dropped a paratrooper
boolean checkForDroppedParatroopers()
{
  byte droppedTrooper;
  boolean dropped = false;
  
  for ( byte ndx = 0; ndx < NUM_BOATS; ndx++ )
  {
    // if there's a trooper in a slot but no boat - he's lunch for sharks
    if ( *( boatPos[ ndx ] ) == eOccupied && boatPosition != ndx )
    {
      *( boatPos[ ndx ] ) = eEmpty;
      dropped = true;
      droppedTrooper = ndx;
      currentParatroopers--;
    }
  }
  
  if ( dropped )
  {
    // update display to remove dropped tropper
    drawParatroopers( currentColumn );
    runSharkAttackAnimation( droppedTrooper );
  }
    
  return ( dropped );
}

// chuck another paratrooper out the heli if it's time
void createParatrooper()
{
  // don't overcrowd the sky
  if ( currentParatroopers < maxParatroopers )
  {
    // 30% chance of more human rain, but not if it just rained - also there's no rain then rain!
    if ( ( random( 10 ) < 3 && skyValues[ columnLoopIndexes[ currentColumn ] + 1 ] != eOccupied ) || currentParatroopers == 0 )
    {
      skyValues[ columnLoopIndexes[ currentColumn ] ] = eOccupied;
      currentParatroopers++;
    }
  }
}

// behold - gravity!
void moveParatroopers()
{
  boolean moved = false;
  
  // with game B paratroopers can get stuck in the palm tree for a bit
  if ( selectedGame == eGameB && currentColumn == 2 )
  {
    if ( skyValues[ TREE_1 ] == eOccupied || skyValues[ TREE_2 ] == eOccupied )
    {
      SkyContentValue tmp;
      
      // swing paratrooper to the opposite branch
      tmp = skyValues[ TREE_1 ];
      skyValues[ TREE_1 ] = skyValues[ TREE_2 ];
      skyValues[ TREE_2 ] = tmp;
      
      // 1 in 3 chance of popping out of tree, but only if it's clear to do so
      if ( skyValues[ NEAR_TREE ] == eEmpty && skyValues[ TREE_1 ] == eOccupied && skyValues[ ABOVE_TREE ] == eEmpty &&
           skyValues[ BELOW_TREE ] == eEmpty && random( 3 ) == 0 )
      {
        skyValues[ TREE_1 ] = eEmpty;
        skyValues[ NEAR_TREE ] = eOccupied;
      }
    }
    else if ( skyValues[ NEAR_TREE ] == eOccupied && random( 4 ) == 0 )
    {
      // 1 in 4 chance came up for this guy - snagged!
      skyValues[ NEAR_TREE ] = eEmpty;
      skyValues[ TREE_1 ] = eOccupied;
    }
  }
  
  // shuffle the current column down - this ignores anyone in the tree
  for ( int ndx = columnLoopIndexes[ currentColumn + 1 ] - 1; ndx > columnLoopIndexes[ currentColumn ]; ndx-- )
  {
    skyValues[ ndx ] = skyValues[ ndx - 1 ];
    if ( skyValues[ ndx ] == eOccupied )
      moved = true;
  }
  
  // if we moved a paratrooper then give a little shout
  if ( moved )
    TV.tone( 700, 25 );
    
  // empty the top spot in the column
  skyValues[ columnLoopIndexes[ currentColumn ] ] = eEmpty;
}

// Simple formatter of numeric values
void BufferPrint( char *buffer, unsigned long value, unsigned long i, int bufLen )
{
  unsigned long temp;
  int bufindex = 0;
 
  if ( buffer )
  {
    memset( buffer, 0, bufLen );
    while ( i > 0 )
    {
      temp = value / i;
      value -= temp * i;
      buffer[ bufindex++ ] = (char)( '0' + temp );
      i/=10;
    }
  }
}

// yup, displays the current score in the top left
void displayScore()
{
  char buf[ 4 ];
  BufferPrint( buf, score, 100, 4 );
  TV.select_font( font8x8 );
  TV.print( 0, 0, buf );
}

void checkResetMisses()
{
  if ( misses && ( score == 200 || score == 500 ) )
  {
    TV.tone( 1000, 50 );
    TV.delay( 250 );
    TV.tone( 1000, 50 );
    TV.delay( 250 );
    TV.tone( 1000, 50 );
    TV.delay( 250 );
    TV.tone( 1500, 300 );
    erasebitmap( misses_positions[ 0 ], MISSES_Y, missshark );
    erasebitmap( misses_positions[ 1 ], MISSES_Y, missshark );
    erasebitmap( misses_positions[ 2 ], MISSES_Y, missshark );
    misses = 0;
  }
}

// increments the max paratroopers allowed on screen depending on score
void updateLevel()
{
  // the more you catch the more you get to catch!
  if ( score % 10 == 0 )
    maxParatroopers++;
  
  // every 100 points the max drops a little before starting to increase again
  if ( score % 100 == 0 )
    maxParatroopers = ( score / 100 ) + 1;
  
  // but let's not go crazy here
  if ( maxParatroopers > 10 )
    maxParatroopers = 10;
}

// did we catch one? increment score and adjust level as required
void checkForSavedParatroopers()
{
  if ( *( boatPos[ boatPosition ] ) == eOccupied )
  {
    *( boatPos[ boatPosition ] ) = eSaved;
    score++;
    if ( score > 999 )
      score = 0;
    TV.tone( 500, 80 );
    displayScore();
    updateLevel();
  }
}

// called once just to set
void initRoamingShark()
{
  currentSharkPos = 0;
  sharkTimer = 0;
  overlaybitmap( shark_positions[ currentSharkPos ].x, shark_positions[ currentSharkPos ].y, shark1_bitmap );
}

// move the shark around in the water
void animateRoamingShark()
{  
  // shark updates every so often
  if ( ++sharkTimer >= random( 2, 5 ) )
  {
    erasebitmap( shark_positions[ currentSharkPos ].x, shark_positions[ currentSharkPos ].y, shark_bitmaps[ currentSharkPos ] );
    currentSharkPos = ( currentSharkPos + 1 ) % NUM_SHARKS;
    overlaybitmap( shark_positions[ currentSharkPos ].x, shark_positions[ currentSharkPos ].y, shark_bitmaps[ currentSharkPos ] );
    sharkTimer = 0;
  }
}

// loop through the attack frames from where the trooper dropped in the drink
void runSharkAttackAnimation( int droppedTrooper )
{
  long t;
  byte index[ 3 ] = { 2, 1, 0 };
  
  TV.tone( 300, 120 );

  // clear the water first
  for ( byte ndx = 0; ndx < NUM_SHARKS; ndx ++ )
    erasebitmap( shark_positions[ ndx ].x, shark_positions[ ndx ].y, shark_bitmaps[ ndx ] );
    
  for ( byte ndx = index[ droppedTrooper ]; ndx < NUM_VICTIMS; ndx++ )
  {
    // no shark in the first frame
    if ( ndx > 0 )
      overlaybitmap( shark_positions[ ndx - 1 ].x, shark_positions[ ndx - 1 ].y, shark_bitmaps[ ndx - 1 ] );
    overlaybitmap( victim_positions[ ndx ].x, victim_positions[ ndx ].y, victim_bitmaps[ ndx ] );
 
    TV.tone( 300, 90 );
    t = TV.millis();
    while ( TV.millis() < t + ( ndx == NUM_VICTIMS - 1 ? 750 : 500 ) )  // show the last frame for an extra quarter second
      // let user move boat while attack animation runs
      getInput();
 
    if ( ndx > 0 )
      erasebitmap( shark_positions[ ndx - 1 ].x, shark_positions[ ndx - 1 ].y, shark_bitmaps[ ndx - 1 ] );
    erasebitmap( victim_positions[ ndx ].x, victim_positions[ ndx ].y, victim_bitmaps[ ndx ] );
  }
}

// we caught one so he can be removed from the sky
void removeSavedParatroopers()
{
  for ( byte ndx = 0; ndx < NUM_BOATS; ndx++ )
  {
    if ( *( boatPos[ ndx ] ) == eSaved )
    {
      *( boatPos[ ndx ] ) = eEmpty;
      currentParatroopers--;
    }
  }
}
