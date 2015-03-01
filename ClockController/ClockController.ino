#include <Wire.h>
#include "Arduino.h"
#include "Constants.h"
#include "Digits.h"
#include "Clock.h"
#include "SimpleTimer.h"

Clock clock(10,11,12); //data, clock, latch
SimpleTimer timer;

uint8_t i2cLocalBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile boolean receivedMessage = false;

void setup() {
    Wire.begin(CLOCK_I2C_ADDR);
    Wire.onReceive(receiveI2CEvent);
    
    timer.setInterval(1000, oneSecondLater);
    clock.startClock();
}

void oneSecondLater(){
    clock.oneSecondLater();
}

void loop() {
  if(receivedMessage){
    copyToI2CLocalBuffer();    
    clock.handleI2CMessage(I2C_MESSAGE_MAX_SIZE, i2cLocalBuffer);
    clearI2CLocalBuffer();
    receivedMessage = false; 
  }
  timer.run();
}

void clearI2CLocalBuffer(){
  for(int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
    i2cLocalBuffer[i] = 0;
  }
}

void copyToI2CLocalBuffer(){
  for(int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
    i2cLocalBuffer[i] = i2cLocalBuffer[i]; 
  }
}

void receiveI2CEvent(int howMany){  
  if(I2C_MESSAGE_MAX_SIZE+1 > howMany){ 
    for(int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
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
