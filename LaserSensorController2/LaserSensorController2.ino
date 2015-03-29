#include <Wire.h>
#include <SFE_TSL2561.h>
#include "Constants.h"
#include "LaserSensorController.h"

int sensorControllerId = 1;
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
    
  attachInterrupt(1, pin3Interrupted, FALLING);
  //attachInterrupt(0, pin2Interrupted, FALLING);
  attachInterrupt(4, pin19Interrupted, FALLING);
  
  laserSensorController.setSensorPin(4, 3, TSL2561_ADDR_0);
  laserSensorController.calibrateSensorBySensorId(4);
  
  //laserSensorController.setSensorPin(5, 2, TSL2561_ADDR);
  //laserSensorController.calibrateSensorBySensorId(5);
  
  laserSensorController.setSensorPin(6, 19, TSL2561_ADDR_1);
  laserSensorController.calibrateSensorBySensorId(6);

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
    laserSensorController.pinInterrupted(2);
    noInterrupts();
    pin2Interrupt = false; 
    interrupts();
  }
  
  if(pin3Interrupt){
    laserSensorController.pinInterrupted(3);
    noInterrupts();
    pin3Interrupt = false; 
    interrupts();
  }
  
  if(pin19Interrupt){
   laserSensorController.pinInterrupted(19);
    noInterrupts();
    pin19Interrupt = false; 
    interrupts();
  }
}

void copyToI2CLocalBuffer(){
  for(int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
    i2cLocalBuffer[i] = i2cDataBuffer[i]; 
  }
}

void receiveI2CEvent(int howMany){    
  for(int i=0; i<howMany && i< I2C_MESSAGE_MAX_SIZE; i++){
    i2cDataBuffer[i] = Wire.read();
  }
  receivedI2CMessage = true;
}
