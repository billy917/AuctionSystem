#include <Wire.h>
#include <XBee.h>
#include "Constants.h"
#include "NFCManager.h"
//Router xBee address - 40c04f18
//I2C address - 1  
  
XBee xbee = XBee();
volatile uint8_t i2cData[3] = {0,0,0};
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
  if(3 == howMany){
    i2cData[0] = Wire.read();
    i2cData[1] = Wire.read(); 
    i2cData[2] = Wire.read();
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
    nfcManager.handleI2CMessage(3, i2cData);
    receivedMessage = false; 
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



