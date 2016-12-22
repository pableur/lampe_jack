#include "WS2801.h"
#include "SPI.h"
#include <Adafruit_NeoPixel.h> 
#include <avr/wdt.h>

const uint8_t ledHead  = 6; 
const uint8_t ledHand  = 5; 
const uint8_t pinInter = 2;
const int periode = 800;

int oldRed=random(200,255);
int oldGreen=random(200,255);
int oldIntensite=100;

// Set the first variable to the NUMBER of pixels. 5 = 5 pixels in a row
Adafruit_NeoPixel stripHead = Adafruit_NeoPixel(2, ledHead, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripHand = Adafruit_NeoPixel(1, ledHand, NEO_GRB + NEO_KHZ800);

bool flagInterrupt=0;
volatile int timeLastInterrupt=0;

volatile boolean changeFonction=false,flagDataRecept=false;

long lastTime=0,fireLastTime=0;
int fireTime=100;
int wait=20;
int rainbowColor=0,fireRainbowColor=0;
bool enable_hand=false;
bool buttonState = LOW;
//déclaration fonction 
void off();
void White();
void Red();
void BlinkRed();
void rainbow();

void compteurInter();


void (*Fonction[])(void)={off, White, Red, BlinkRed, rainbow};
String nameFonction[]={"off","White","red","BlinkRed", "rainbow"};
volatile int numFonction=0;
volatile int sizeFonction=4;

void setup() {
  pinMode(pinInter,INPUT);
  digitalWrite(pinInter,HIGH);  //résitance de tirage
  
  Serial.begin(9600);
  stripHead.begin();
  stripHand.begin();
 
  // Update LED contents, to start they are all 'off'
  Fonction[0]();
}

void loop() {
  //Serial.println(millis());
  fireHand();
  (Fonction[numFonction])();
  if(buttonState==HIGH && digitalRead(pinInter)==LOW){
    //delay(500); //stabilise l'entrée
    numFonction++;
    if(numFonction>sizeFonction){
      numFonction=0;    
    }
    Serial.println("change fonction : ");
    Serial.println(nameFonction[numFonction]);    
     (Fonction[numFonction])();
    enable_hand=true;   
    delay(500); //stabilise l'entrée
   }
   buttonState=digitalRead(pinInter);
   delay(10);
}

void fireHand(){
  if(enable_hand){
    if(millis()-fireLastTime>(fireTime)){
      fireLastTime=millis();
      fireTime=random(20,200);
      int green=(random(0,255)*2+oldGreen*8)/10;
      if (green>255)
        green=255;
      if (green<0)
        green=0;

      oldGreen=green;
      //Serial.println(green);
      int intensite=(random(0,100)*3+oldIntensite*7)/10;
      stripHand.setPixelColor(0, Color((255*intensite)/100,(green*intensite)/100,0));
      stripHand.show();   // write all the pixels out      
      }
  }
}


/////////////////////////////////////
//  etat fonction
//////////////////////////////////////

void rainbow() {
  if(millis()-lastTime>(wait-10)){
    lastTime=millis();
    int i; 
    rainbowColor++;
    if(rainbowColor>255)
      rainbowColor=0;
    int j = rainbowColor;
      // 3 cycles of all 256 colors in the wheel
      for (i=0; i < stripHead.numPixels(); i++) {
        stripHead.setPixelColor(i, Wheel( (i + j) % 255));
      }  
      stripHead.show();   // write all the pixels out
  }
}

void White(){
  colorWipe (Color(255,255,255),0);
}
void off(){  
  colorWipe (Color(0,0,0),0);
  stripHand.setPixelColor(0, Color(0,0,0));
  stripHand.show();
  delay(wait);
  enable_hand=false;
}

void Red(){
  colorWipe(Color(255,0,0),0);
}

void BlinkRed(){
  if(millis()-lastTime<periode){
    colorWipe (Color(255,255,255),0);
  }else if(millis()-lastTime<periode*2){
    colorWipe (Color(255,0,0),0);
  }else{
  lastTime=millis();
  }
}
/////////////////////////////////////
//  led fonction
//////////////////////////////////////

// fill the dots one after the other with said colorà
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < stripHead.numPixels(); i++) {
      stripHead.setPixelColor(i, c);
      stripHead.show();
      delay(wait);
  }
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte g,byte r, byte b){
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos){
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

