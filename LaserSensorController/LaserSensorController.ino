#include <Wire.h>
#include <SFE_TSL2561.h>
#include "Constants.h"
#include "LaserSensorController.h"

int sensorControllerId = 2;
LaserSensorController laserSensorController(sensorControllerId, false);
uint8_t i2cLocalBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile boolean receivedMessage = false;

void setup() {
  Serial.begin(9600);
  Wire.begin(LASER_SENSOR_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
    
  delay(500);
  attachInterrupt(0, pin2Interrupted, FALLING);
  attachInterrupt(1, pin3Interrupted, FALLING);
  attachInterrupt(4, pin19Interrupted, FALLING);
  
  laserSensorController.setSensorPin(1, 2, TSL2561_ADDR_0);
  delay(1000);

  laserSensorController.calibrateSensorBySensorId(1);
}

void pin2Interrupted(){
  Serial.println("Pin 2 Interrupted");
  laserSensorController.pinInterrupted(2);
}

void pin3Interrupted(){
  laserSensorController.pinInterrupted(3);
}

void pin19Interrupted(){
  laserSensorController.pinInterrupted(19);
}

void loop() {
  if(receivedMessage){
    copyToI2CLocalBuffer();    
    laserSensorController.handleMessage(I2C_MESSAGE_MAX_SIZE, i2cLocalBuffer);
    clearI2CLocalBuffer();
    receivedMessage = false; 
  }
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
