#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

byte frame[8][12] = {
  {0,0,1,1,1,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,0,0,0,0,0,0},
  {0,0,1,1,1,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,0,0,0,0,0,0},
  {0,0,1,1,1,0,1,1,1,0,0,0},
  {0,0,1,0,1,0,1,0,1,0,0,0},
  {0,0,1,0,1,0,1,0,1,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};

byte zeroframe[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};


void setup() {
  Serial.begin(115200);

}


void loop() {
  for(int i=0; i<8; i++){
    for (int j=0; j<12; j++){
      Serial.write(frame[i][j]);
    }
  }
  delay(1000);
    for(int k=0; k<8; k++){
    for (int l=0; l<12; l++){
      Serial.write(zeroframe[k][l]);
    }
  }
  delay(1000);

}

