//Coordinator xBee address - 40c337e0
//I2C address: 1

#include <Wire.h>
#include <Servo.h>
#include <XBee.h>
#include "Constants.h"
#include "LaserController.h"
 
// Pin 13 has an LED connected on most Arduino boards.

int laserPins[3] = {4,8,12};
unsigned long timer = 0;

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();

uint8_t xbeePayload[4] = { 0, 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));

int laserControllerId = 1;
bool enableSensors = true;
LaserController laserController(laserControllerId, false,enableSensors);

volatile bool receivedI2CMessage = false;
volatile bool receivedXBeeMessage = false;
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t xBeeDataBuffer[I2C_MESSAGE_MAX_SIZE];
uint8_t localBuffer[I2C_MESSAGE_MAX_SIZE];

// the setup routine runs once when you press reset:
void setup() {           

  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  for(int i=1; i<=3; i++){
    pinMode(laserPins[i-1], OUTPUT);
    digitalWrite(laserPins[i-1], LOW);  
    laserController.setLaserPin((laserControllerId * 3)+i, laserPins[i-1]);  
  }
  
  Wire.begin(NFC_MANAGER_I2C_ADDR);
  Wire.onReceive(receiveEvent);
  xbee.begin(Serial);   
  laserController.setXBeeReference(&xbee);
}

void loop() {
  handleXBeeMsg();
  if(receivedXBeeMessage){
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      localBuffer[i] = xBeeDataBuffer[i];
    }
    handleMessage();
    receivedXBeeMessage = false; 
  }

  if(receivedI2CMessage){
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      localBuffer[i] = i2cDataBuffer[i];
    }
    handleMessage();
    receivedI2CMessage = false; 
  }
}

void receiveEvent(int howMany){
  for(int i=0; i<howMany && i< I2C_MESSAGE_MAX_SIZE; i++){
    i2cDataBuffer[i] = Wire.read();
  }
  receivedI2CMessage = true;
}

void handleXBeeMsg(){
  xbee.readPacket();
  if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);
    xBeeDataBuffer[0] = rx.getData(0);
    if(MESSAGETYPEID_LASER_CONTROL == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    }
    receivedXBeeMessage = true;
  } 
}

void handleMessage(){
  if(laserController.canHandleMessageType(localBuffer[0])){
    laserController.handleMessage(3, localBuffer);
  } 
 
  if(localBuffer[0] == 1){
    //Turn Off        
    laserController.turnOffAllLaser();    
  } else if (localBuffer[0] == 3){
    //Turn On      
    laserController.turnOnAllLaser();
  }   
}


