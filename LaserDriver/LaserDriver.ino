//Coordinator xBee address - 40c04edf
//I2C address: 1

#include <Wire.h>
#include <XBee.h>
#include "Constants.h"
#include "LaserController.h"
#include "NFCManager.h"
 
// Pin 13 has an LED connected on most Arduino boards.

int laserPins[3] = {4,8,12};

unsigned long timer = 0;

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
uint8_t xbeePayload[4] = { 0, 0, 0, 0 };
XBeeAddress64 laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1);
ZBTxRequest laser2Tx = ZBTxRequest(laser2Addr, xbeePayload, sizeof(xbeePayload));


int laserControllerId = 0;
int nfcManagerId = 0;
bool enableSensors = false;
LaserController laserController(laserControllerId, true, enableSensors);
NFCManager nfcManager(nfcManagerId, true);

volatile char commandSource = ' ';
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
  nfcManager.setXBeeReference(&xbee);
  nfcManager.setLockManagerI2CAdder(LOCK_MANAGER_I2C_ADDR);
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
  commandSource = 'I';
  receivedI2CMessage = true;
}

void handleXBeeMsg(){
  xbee.readPacket();
  if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);
        
    xBeeDataBuffer[0] = rx.getData(0);    
    if(5 == xBeeDataBuffer[0] ||
        6 == xBeeDataBuffer[0] ||
        MESSAGETYPEID_LASER_CONTROL == xBeeDataBuffer[0] ||
        MESSAGETYPEID_LASER_SENSOR == xBeeDataBuffer[0] ||
        MESSAGETYPEID_LOCK == xBeeDataBuffer[0] ||
        MESSAGETYPEID_BGM == xBeeDataBuffer[0] ||
        MESSAGETYPEID_KEYPAD_LOCK == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    } else if(MESSAGETYPEID_NFC_MANAGE == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
      xBeeDataBuffer[3] = rx.getData(3);
      xBeeDataBuffer[4] = rx.getData(4);
    }
    
    commandSource = 'X';
    receivedXBeeMessage = true;
  }   
}


void forwardXBeeMessage(){
  xbeePayload[0] = localBuffer[0];
  xbeePayload[1] = localBuffer[1];
  xbeePayload[2] = localBuffer[2];
  xbeePayload[3] = localBuffer[3];  
  xbee.send(laser2Tx);
}

void forwardI2CMessage(int i2cAddr){
  Wire.beginTransmission(i2cAddr); //4 == ShelfLock
  Wire.write(localBuffer[0]);
  Wire.write(localBuffer[1]);
  Wire.write(localBuffer[2]);
  Wire.write(localBuffer[3]);
  Wire.write(localBuffer[4]);
  Wire.endTransmission();
}

void handleMessage(){
  if(laserController.canHandleMessageType(localBuffer[0])){
      laserController.handleMessage(3, localBuffer);
  } 
  
  if(MESSAGETYPEID_LOCK == localBuffer[0]){
    forwardI2CMessage(LOCK_MANAGER_I2C_ADDR);
  } else if(MESSAGETYPEID_BGM == localBuffer[0]){
    forwardI2CMessage(BGM_I2C_ADDR);
  } else if (MESSAGETYPEID_KEYPAD_LOCK == localBuffer[0]){
    forwardI2CMessage(KEYPAD_LOCK_I2C_ADDR);
  } else if (MESSAGETYPEID_NFC_MANAGE == localBuffer[0]){
    forwardI2CMessage(KEYPAD_LOCK_I2C_ADDR);
  } else if (MESSAGETYPEID_LASER_SENSOR == localBuffer[0]){
    forwardI2CMessage(LASER_SENSOR_I2C_ADDR);
  } else if (MESSAGETYPEID_CLOCK == localBuffer[0]){
    forwardXBeeMessage();
  }

  nfcManager.handleI2CMessage(I2C_MESSAGE_MAX_SIZE, localBuffer);
  
  if(localBuffer[0] == 1){
    //Turn Off
    laserController.turnOffAllLaser();
  } else if (localBuffer[0] == 3){
    //Turn On
    laserController.turnOnAllLaser();
  } 
  commandSource = ' ';
}

