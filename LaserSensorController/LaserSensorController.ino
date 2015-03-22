#include <Wire.h>
#include <SFE_TSL2561.h>
#include "Constants.h"
#include "LaserSensorController.h"

int sensorControllerId = 0;
LaserSensorController laserSensorController(sensorControllerId, false);
uint8_t i2cLocalBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile boolean receivedMessage = false;

volatile boolean pin2Interrupt, pin3Interrupt, pin19Interrupt = false;
void pin2Interrupted(){
  pin2Interrupt = true;
}

void pin3Interrupted(){
  pin3Interrupt = true;
}

void pin19Interrupted(){
  pin19Interrupt = true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(LASER_SENSOR_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
    
  attachInterrupt(0, pin2Interrupted, FALLING);
  //attachInterrupt(1, pin3Interrupted, FALLING);
  //attachInterrupt(4, pin19Interrupted, FALLING);
  
  laserSensorController.setSensorPin(1, 2, TSL2561_ADDR
  u);
  laserSensorController.calibrateSensorBySensorId(1);
  
  //laserSensorController.setSensorPin(2, 3, TSL2561_ADDR_0);
  //laserSensorController.calibrateSensorBySensorId(2);
}

void loop() {
  if(receivedMessage){
    noInterrupts();
    copyToI2CLocalBuffer();    
    receivedMessage = false; 
    interrupts();   
    laserSensorController.handleMessage(I2C_MESSAGE_MAX_SIZE, i2cLocalBuffer);
  }

  if(pin2Interrupt){
    noInterrupts();
    pin2Interrupt = false; 
    interrupts();
    laserSensorController.pinInterrupted(2);
  }
  
  if(pin3Interrupt){
    noInterrupts();
    pin3Interrupt = false; 
    interrupts();
    laserSensorController.pinInterrupted(3);
  }
  
  if(pin19Interrupt){
    noInterrupts();
    pin19Interrupt = false; 
    interrupts();
    laserSensorController.pinInterrupted(19);
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
