// LOL Shield GPS by James Wood http://j-w.co
// Based on code and hardware by Jimmie P. Rogers http://jimmieprodgers.com/
// and code by Mikal Hart http://arduiniana.org


#include "Charliplexing.h"
#include <TinyGPS++.h>
unsigned int speedDisp = 0, satsDisp = 0, hdopDisp = 0, brightness = 1, brightnessDisp = 1;
boolean doUpdate = true;
unsigned long lastUpdate = 0, lastBrightness = 0;

const byte digits[][4] = {
  {14, 17, 17, 14}, //0
  { 0, 18, 31, 16}, //1
  {18, 25, 21, 18}, //2
  {17, 21, 21, 10}, //3
  {12, 10, 31,  8}, //4
  {23, 21, 21,  9}, //5
  {14, 21, 21,  8}, //6
  { 1, 25,  5,  3}, //7
  {31, 21, 21, 31}, //8
  {18, 21, 21, 14}, //9
};

TinyGPSPlus gps;

void setup(){
  Serial.begin(9600);
  LedSign::Init(DOUBLE_BUFFER | GRAYSCALE);
}

void loop(){  
  while (Serial.available() > 0) { gps.encode(Serial.read()); }
  
  if (lastBrightness + 2000 < millis()){
    brightness = map(analogRead(5), 0, 900, 1, 7);
    lastBrightness = millis();
    doUpdate = true;
  }
  
  if (gps.speed.isUpdated()) {
    if (speedDisp != int(gps.speed.kmph())) {
      speedDisp = int(gps.speed.kmph());
      doUpdate = true;
    }
  }

  if (gps.satellites.isUpdated()) {
    if(satsDisp != gps.satellites.value()) {
      satsDisp = gps.satellites.value();
      doUpdate = true;
    }
  }

  if (gps.hdop.isUpdated()) {
    int hdopNew = int(gps.hdop.value());
    if (hdopNew >= 200) hdopNew = 0; else if(hdopNew <= 10) hdopNew = 10; else hdopNew = map(hdopNew,10,200,14,0);
    if (hdopNew != hdopDisp) {
      hdopDisp = hdopNew;
      doUpdate = true;
    }
  }

  if (brightness != brightnessDisp) {
    doUpdate=true;
    brightnessDisp = brightness;
  }
  
  if (doUpdate) {
    for (int row=0; row<9; row++)  for (int col=0; col<14; col++) LedSign::Set(col, row, byte(0));
    drawSpeed(speedDisp);
    drawBar(satsDisp, 0);
    drawBar(hdopDisp, 8);
    LedSign::Flip(false);
    lastUpdate = millis();
    doUpdate = false;
  }
}

void drawSpeed(unsigned int intToDisplay){
  String numberToDisplay = String(intToDisplay);
  int numberOfChars = numberToDisplay.length();
  int charOffset = (15-(5*numberOfChars))/2;
  for (int digitToDraw=0; digitToDraw<numberOfChars; digitToDraw++){
    int thisDigit = int(numberToDisplay.charAt(digitToDraw))-48;
    for (int col=0; col<4; col++){
      for (int row=0; row<5; row++){
        LedSign::Set(col+(5*digitToDraw)+charOffset, row+2, byte(bitRead(digits[thisDigit][col],row) * brightness));
      }
    }
  }
}

void drawBar(unsigned int number, int row){
  int isLit;
  for (int i = 0; i < 14; i++){
    if (number > i){isLit = brightness; } else {isLit = 0;};
    LedSign::Set(i, row, byte(isLit));
  }
}
