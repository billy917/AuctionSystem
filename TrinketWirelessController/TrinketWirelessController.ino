
#include <TinyWireM.h> 

//PIN 0, 2 - I2C
//PIN 1 = A, 3 = B, 4 = C
int MODE_OFF = 0;
int MODE_ON = 1;
int MODE_RESET = 2;

int numSignals = 3;
int signalPin[3] = {1, 3, 4};
int lastSignalState[3] = {LOW, LOW, LOW};
int curSignalState[3];

void setup() {                
   pinMode(1,INPUT);
   pinMode(3,INPUT);
   pinMode(4,INPUT);
   TinyWireM.begin();
   
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
  TinyWireM.beginTransmission(4);
  TinyWireM.send(mode);              // sends one byte  
  TinyWireM.endTransmission();    // stop transmitting
  
  TinyWireM.beginTransmission(5);
  TinyWireM.send(mode);              // sends one byte  
  TinyWireM.endTransmission();    // stop transmitting
}
