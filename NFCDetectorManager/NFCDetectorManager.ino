#include <Wire.h>
#include <XBee.h>
#include "Constants.h"
#include "NFCManager.h"
//Router xBee address - 40c04f18
//I2C address - 1  
  
XBee xbee = XBee();
volatile uint8_t i2cDataBuffer[NFC_MESSAGE_MAX_SIZE];
volatile boolean receivedMessage = false;
// need to store expected UID for each detector

int led = 13;
NFCManager nfcManager(3, false);

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  Wire.begin(NFC_MANAGER_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
  xbee.begin(Serial);  
  nfcManager.setXBeeReference(&xbee);
}

void receiveI2CEvent(int howMany){  
  if(NFC_MESSAGE_MAX_SIZE+1 > howMany){ // assume 9 is the max size of XBee message
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      if(i>=howMany){
        i2cDataBuffer[i] = 0;
      } else {
        i2cDataBuffer[i] = Wire.read(); 
      }
    }
    receivedMessage = true; // TODO: need better way to queue incoming requests
  } else {
    while(1 < Wire.available()) // loop through all but the last
    {
      char c = Wire.read(); // receive byte as a character
      Serial.print(c);         // print the character
    }
  }
}

void loop() {
  if(receivedMessage){
    uint8_t data[NFC_MESSAGE_MAX_SIZE] = {};
    for(int i=0;i<NFC_MESSAGE_MAX_SIZE;i++){
      data[i] = i2cDataBuffer[i]; 
    }  
    nfcManager.handleI2CMessage(9, data);

    clearI2CBuffer();
    receivedMessage = false; 
  }
}

void clearI2CBuffer(){
  for(int i=0; i<UID_LENGTH; i++){
    i2cDataBuffer[i] = 0;
  }
}

void blinkLED(int times){
  for(int i=0; i<times; i++){
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(500);     
  }
}



