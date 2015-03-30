#include <Wire.h>
#include <SFE_TSL2561.h>
#include "Constants.h"
#include "LaserSensorController.h"

int sensorControllerId = 0;
LaserSensorController laserSensorController(sensorControllerId, false);
uint8_t i2cLocalBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile boolean receivedI2CMessage = false;

volatile boolean pin2Interrupt = false;
volatile boolean pin3Interrupt = false;
volatile boolean pin19Interrupt = false;

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
    
  //attachInterrupt(1, pin3Interrupted, FALLING); // Laser5, Sensor 1
  //attachInterrupt(0, pin2Interrupted, FALLING); // Laser?, Sensor2 
  attachInterrupt(4, pin19Interrupted, FALLING); // Laser2, Sensor3
  
  
  Wire.begin(LASER_SENSOR_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
  
  //laserSensorController.setSensorPin(1, 3, TSL2561_ADDR_0);
  
  //laserSensorController.setSensorPin(2, 2, TSL2561_ADDR);
  
  laserSensorController.setSensorPin(3, 19, TSL2561_ADDR_1);
  Serial.println("Initialized");
}

void loop() {
  if(receivedI2CMessage){
    Serial.println("Received I2C message");
    noInterrupts();
    copyToI2CLocalBuffer();    
    receivedI2CMessage = false; 
    interrupts();   
    handleMessage();    
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

void handleMessage(){
  laserSensorController.handleMessage(I2C_MESSAGE_MAX_SIZE, i2cLocalBuffer);
  laserSensorController.printState();
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
