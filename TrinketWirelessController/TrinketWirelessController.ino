//I2C address: 2
#include <Wire.h> 

//PIN 0, 2 - I2C
//PIN 1 = A, 3 = B, 4 = C
int MODE_OFF = 1;
int MODE_RESET = 2;
int MODE_ON = 3;

int numSignals = 4;
int signalPin[4] = {3, 4, 5, 6};
int lastSignalState[4] = {LOW, LOW, LOW, LOW};
int curSignalState[4];

void setup() {                
   pinMode(3,INPUT);
   pinMode(4,INPUT);
   pinMode(5,INPUT);
   pinMode(6,INPUT);
   Wire.begin();
   
   readCurrentSignalState();
   copyLastSignalState();
   delay(1000);
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
        send(i);
      }
    } 
  }
  copyLastSignalState();
  
  delay(200);
}

void send(int mode){
  Wire.beginTransmission(1);
  Wire.write(mode);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  
  Wire.beginTransmission(3);
  Wire.write  (mode);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
}
