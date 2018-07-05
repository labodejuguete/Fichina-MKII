/*
Mário Vairinhos 2012
This software is free. Anyone may distribute, copy or modify
as long as he or she gives me credits.
*/

#include <TVout.h>
#include <fontALL.h>
#include "rocket.h"
TVout TV;

//#define TVSYSTEM        _PAL
#define TVSYSTEM        _NTSC
#define SCREENWIDTH     128
#define SCREENHEIGHT    96
#define SCREENWIDTH2    64
#define SCREENHEIGHT2   48
#define ASTEROIDS       18

// pin controls
#define BT_UP           4
#define BT_DOWN         5
#define BT_LEFT         3
#define BT_RIGHT        2

#define BEEP      TV.tone(1046,200);

// globals
int asteroidsX[ASTEROIDS];
int asteroidsY[ASTEROIDS];
int asteroidsZ[ASTEROIDS];
float meX, meY,meZ=0;
float screenX,screenY;
float myspeed;
int level;
int energy;
int tick;
int seconds;
int frames;
byte gameState=0; //(0-startLevel,1-run,2-gameOver,3 win)



void setup()
{

  //init TV library
  TV.begin(TVSYSTEM, SCREENWIDTH, SCREENHEIGHT);
  TV.select_font(font4x6);
  TV.delay_frame(60);
  TV.clear_screen();
  
  // setup pullup resistors
  pinMode(BT_UP   ,INPUT);digitalWrite(BT_UP   ,HIGH);
  pinMode(BT_DOWN ,INPUT);digitalWrite(BT_DOWN ,HIGH);
  pinMode(BT_LEFT ,INPUT);digitalWrite(BT_LEFT ,HIGH);
  pinMode(BT_RIGHT,INPUT);digitalWrite(BT_RIGHT,HIGH);

  level=0;
  gameState=0;
}


// game's state
void loop()
{
 switch (gameState){
  case 0:
    startLevel();
    break;
  case 1:
    run();
    break;
  case 2:
    run();
    break;
  case 3:
    win();
    break;    
    
 } 
}




// static vars
byte startLev=0;
byte ry,rdy;

void startLevel(){
  // random asteroids once
  if (startLev==0){
    BEEP
    meX=100;meY=0;meZ=0;startLev=1;ry=20;rdy=1;
    for(int i=0;i<ASTEROIDS;i++){asteroidsX[i]=random(130,230);asteroidsY[i]=random(1,95);asteroidsZ[i]=random(1,5);} 
  }

  // render the screen
  // and move the rocket
  tempo();
  frames++;
  if (frames>2){
   TV.clear_screen();
   frames=0;
   ry+=rdy;
   if (ry>20) rdy=-1;
   if (ry<5) rdy=1;   
   TV.bitmap(25,ry,rocket,1,72,40);
   drawLevel();
  }


    // move the asteroids
    for(int i=0;i<ASTEROIDS;i++){
      asteroidsX[i]-=asteroidsZ[i];
      if(asteroidsX[i]<=1) asteroidsX[i]=130;
      if (asteroidsX[i]<128 && asteroidsX[i]>0) TV.set_pixel(asteroidsX[i],asteroidsY[i],1);
    } 
    
     // check user input
     if ((digitalRead(BT_LEFT) ==LOW) &&  (digitalRead(BT_RIGHT) ==LOW)){
       initLevel();
       gameState=1;
       startLev=0;
       clicka();
     }
   // after 60 seconds on inactivity, start the level anyway
   if (seconds>60) initLevel();
   
   TV.delay_frame(1);
}




void clicka(){
  BEEP
  TV.delay(200);
  TV.invert();
  TV.delay(200);
  TV.clear_screen();
}






void win(){
  level++;
  gameState=0;
  startLev=0;
}

void run()
{

  tempo();
  
  // every 3 frames clear te screen 
  // and render the cockpit
  // every frame render the asteroids
  frames++;
  if (frames>3){
   frames=0;
   TV.clear_screen();
   drawCockpit();
   if (gameState==2){
     drawGameOver();
     if (seconds>10){
       gameState=0;
       level=1;
     }
   }
  }
  
  // user input
  if (digitalRead(BT_LEFT) ==LOW){meX-=1+level/3;}
  if (digitalRead(BT_RIGHT)==LOW){meX+=1+level/3;}
  if (digitalRead(BT_UP) ==LOW){meY-=1+level/3;}
  if (digitalRead(BT_DOWN)==LOW){meY+=1+level/3;}
  
  // collision
    for(int i=0;i<ASTEROIDS;i++){
      if (asteroidsZ[i]<20){
        if (abs(asteroidsX[i]-meX)+abs(asteroidsY[i]-meY)<=25){
          if (gameState==1){
            beepCrash();
            TV.fill(2);
            energy--;
          }
        }
        respawn(i,meX,meY,800,i*20+5,0);
      }
      asteroidsZ[i]-=myspeed;
    }
    renderAsteroids();
    
 if (energy<=0){
   gameState=2;
   energy=1;
   seconds=0;
 }

 
 if (seconds>100){
   win();
 }
}











void initLevel(){
  TV.noTone();
  myspeed=10+level*2; //3
  energy=5;
  tick=0;
  seconds=0;
  meX=0;
  meY=0;
  meZ=0;
  for(int i=0;i<ASTEROIDS;i++){
    respawn(i,meX,meY,1000,120,600); 
  }
  gameState=1; 
  startLev=0;
}


void respawn(int index,int ox, int oy,int oz,int radius,int radiusZ){
  asteroidsX[index]=ox+random(-radius,+radius);
  asteroidsY[index]=oy+random(-radius,+radius);
  asteroidsZ[index]=oz+random(radiusZ);
}
void get2dFrom3d(float coordX,float coordY,float coordZ){
  coordX=coordX-meX;
  coordY=coordY-meY;
  coordZ=coordZ-meZ;
  if (coordZ==0) coordZ=0.1;
    screenX=(float(coordX)/float(coordZ))*SCREENWIDTH;
    screenY=(float(coordY)/float(coordZ))*SCREENHEIGHT;
}

void renderAsteroids(){
   float x1,radius;
   int vx,vy;
   for(int i=0;i<ASTEROIDS;i++){
    get2dFrom3d(asteroidsX[i],asteroidsY[i],asteroidsZ[i]);
    radius=2200/(asteroidsZ[i]<<2);
    vx=SCREENWIDTH2+screenX;
    vy=SCREENHEIGHT2+ screenY;
    if (vx<(SCREENWIDTH-radius-1) && vx>(radius+1) && vy<(SCREENHEIGHT-radius-1) && vy>(radius+1)){
        TV.draw_circle(vx,vy,radius,1,1);
    }
  }
  TV.delay_frame(1);
}

void tempo(){
  tick++;
  if (tick>30){
    tick=0;
    seconds++;
  }
}



void drawCockpit(){
   TV.select_font(font4x6);
   byte x=43;byte y=71;
   TV.print_char(x,y,'E');
   TV.draw_rect (x+6,y+1,energy*2  ,4  ,1,1);
   TV.draw_rect (x+5,y+0,12 ,5  ,1);
   x=65;y=71;
   TV.print_char(x,y,'T');
   TV.draw_rect (x+6,y+1 ,seconds/10  ,4  ,1,1);
   TV.draw_rect (x+5,y   ,12 ,5  ,1);
   
   TV.draw_line(20,SCREENHEIGHT,40,SCREENHEIGHT-16,1);
   TV.draw_line(SCREENWIDTH-20,SCREENHEIGHT,SCREENWIDTH-40,SCREENHEIGHT-16,1);
   TV.draw_rect(40,SCREENHEIGHT-16-12,SCREENWIDTH-40-40,12,1);
   TV.draw_line(0,SCREENHEIGHT-16-12-30,40,SCREENHEIGHT-16-12,1);
   TV.draw_line(SCREENWIDTH,SCREENHEIGHT-16-12-30,SCREENWIDTH-40,SCREENHEIGHT-16-12,1);   

    TV.print_char(55,84,'L');
    TV.print_char(60,84,'E');
    TV.print_char(65,84,'V');
    TV.print_char(72,84,level+48);

}

void drawLevel(){
  byte x,y;
  x=30;y=50;
  TV.select_font(font8x8);
  TV.print(x+15,y,"LEVEL");
  TV.print(byte(level+48));
  TV.select_font(font4x6);
  TV.print(x,y+10,"press LEFT & RIGHT");
}


void drawGameOver(){
  TV.select_font(font8x8);
  TV.print(30,30,"GAME OVER");
  TV.delay_frame(1);
}



void beepCrash(){
 TV.tone(random(60,100),150);
}

