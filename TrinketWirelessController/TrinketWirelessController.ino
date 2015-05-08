//I2C address: 2
#include <Wire.h> 
#include "Constants.h"
#include "GameController.h"

//PIN 0, 2 - I2C
//PIN 1 = A, 3 = B, 4 = C
int MODE_OFF = 1;
int MODE_RESET = 2;
int MODE_ON = 3;

int numSignals = 4;
int signalPin[4] = {3, 4, 5, 6};
int lastSignalState[4] = {LOW, LOW, LOW, LOW};
int curSignalState[4];
GameController gameController;

void setup() {      
   pinMode(13, OUTPUT);
   
   Wire.begin(WIRELESS_I2C_ADDR);  
   pinMode(3,INPUT);
   pinMode(4,INPUT);
   pinMode(5,INPUT);
   pinMode(6,INPUT);
   
   readCurrentSignalState();
   copyLastSignalState();
   blink();
   delay(2000);
   gameController.overrideConfig();
}

void blink(){
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(13, LOW); 
  delay(300);
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(13, LOW);
}

void readCurrentSignalState(){
  for (int i=0; i<numSignals; i++){
    curSignalState[i] = digitalRead(signalPin[i]);
  }
}

void copyLastSignalState(){
  for (int i=0; i<numSignals; i++){
    lastSignalState[i] = curSignalState[i];
  }
}


// the loop routine runs over and over again forever:
void loop() {
  
  // check if there are any signals
  readCurrentSignalState();
  for(int i=0; i<numSignals; i++){
    if(curSignalState[i] != lastSignalState[i]){
      // just pressed
      if(curSignalState[i] == HIGH){
        detectedSignal(i);
      }
    } 
  }
  copyLastSignalState();
  
  delay(200);
}

void detectedSignal(int pinIndex){
  if(0 == pinIndex){ // pin 3 A
    if(GAME_STATE_RUNNING == gameController.getCurrentState()){
      gameController.pause();  
    } else {
      gameController.start();
    }
  } else if (1 == pinIndex){ // pin 4 B
    if(gameController.areSensorsDisabled()){
      gameController.enableSensors();
    } else {
      gameController.disableSensors();
    }    
  } else if (2 == pinIndex){ // pin 5 C
    gameController.addTime();
  } else if (3 == pinIndex){ // pin 6 D
    gameController.restart();    
  }
  blink();
}
