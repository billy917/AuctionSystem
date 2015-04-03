#include <Wire.h>
#include <XBee.h>
#include <SFE_TSL2561.h>
#include "Constants.h"
#include "LaserSensorController.h"

int sensorControllerId = 0;
LaserSensorController laserSensorController(sensorControllerId, false);
uint8_t localBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t xBeeDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile boolean receivedI2CMessage = false;
volatile bool receivedXBeeMessage = false;

volatile boolean pin2Interrupt = false;
volatile boolean pin3Interrupt = false;
volatile boolean pin19Interrupt = false;

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();

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
  Serial2.begin(9600);
  xbee.begin(Serial2);   
  laserSensorController.setXBeeReference(&xbee);
  
  Wire.begin(LASER_SENSOR_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
  
  laserSensorController.setSensorPin(1, 1, 3, TSL2561_ADDR_0, &pin3Interrupted); //0  
  laserSensorController.setSensorPin(2, 0, 2, TSL2561_ADDR, &pin2Interrupted);  
  laserSensorController.setSensorPin(3, 4, 19, TSL2561_ADDR_1, &pin19Interrupted); //1
  Serial.println("Initialized");
}

void loop() {
  laserSensorController.clearTrippedFlag();
  handleXBeeMsg();
  if(receivedXBeeMessage){
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      localBuffer[i] = xBeeDataBuffer[i];
    }
    handleMessage();
    receivedXBeeMessage = false; 
  }
  
  if(receivedI2CMessage){    
    Serial.println("Received I2C message");
    noInterrupts();
    for(int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
      localBuffer[i] = i2cDataBuffer[i]; 
    }
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
  laserSensorController.handleMessage(I2C_MESSAGE_MAX_SIZE, localBuffer);
}

void handleXBeeMsg(){
  xbee.readPacket();
  if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);
        
    xBeeDataBuffer[0] = rx.getData(0);    
    if(MESSAGETYPEID_LASER_SENSOR == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    }
    receivedXBeeMessage = true;
  }   
}

void receiveI2CEvent(int howMany){    
  for(int i=0; i<howMany && i< I2C_MESSAGE_MAX_SIZE; i++){
    i2cDataBuffer[i] = Wire.read();
  }
  receivedI2CMessage = true;
}
