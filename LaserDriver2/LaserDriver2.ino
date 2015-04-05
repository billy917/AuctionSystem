//Coordinator xBee address - 40c04ef1
//I2C address: 2

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

uint8_t xbeePayload[3] = { 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));

XBeeAddress64 laser3Addr = XBeeAddress64(0x0013a200, 0x40c337e0);
ZBTxRequest laser3Tx = ZBTxRequest(laser3Addr, xbeePayload, sizeof(xbeePayload));

int laserControllerId = 2;
int nfcManagerId = 2;
bool enableSensors = true;
LaserController laserController(laserControllerId, false, enableSensors);
NFCManager nfcManager(nfcManagerId, false);

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
    digitalWrite(laserPins[i-1],LOW);
    laserController.setLaserPin((laserControllerId * 3)+i, laserPins[i-1]);
  }

  Wire.begin(NFC_MANAGER_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
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

void receiveI2CEvent(int howMany){
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
    if(5 == xBeeDataBuffer[0] 
        || MESSAGETYPEID_NFC == xBeeDataBuffer[0] 
        || MESSAGETYPEID_LASER_CONTROL == xBeeDataBuffer[0]
        || MESSAGETYPEID_LASER_SENSOR == xBeeDataBuffer[0]
        || MESSAGETYPEID_LOCK == xBeeDataBuffer[0]
        || MESSAGETYPEID_BGM == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    } else if(MESSAGETYPEID_NFC_MANAGE == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
      xBeeDataBuffer[3] = rx.getData(3);
    } else if (MESSAGETYPEID_CLOCK == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      if(MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT == xBeeDataBuffer[1] ||
         MESSAGETYPEID_CLOCK_MODIFY_ADD == xBeeDataBuffer[1]){
        xBeeDataBuffer[2] = rx.getData(2);
        xBeeDataBuffer[3] = rx.getData(3);
      }
    }
    
    commandSource = 'X';
    receivedXBeeMessage = true;
  }   
}

void instructXBeeModeChange(int messageTypeId){
  // send instruction to other xBee on mode change
  xbeePayload[0] = messageTypeId;
  xbeePayload[1] = 0;
  xbeePayload[2] = 0;
  xbee.send(laser1Tx);  
  xbee.send(laser3Tx);
}

void handleMessage(){
  if(laserController.canHandleMessageType(localBuffer[0])){
      laserController.handleMessage(3, localBuffer);
  }
  if(MESSAGETYPEID_CLOCK == localBuffer[0]){    
    forwardI2CMessage(SOUNDFX_I2C_ADDR);
    forwardI2CMessage(CLOCK_I2C_ADDR);
  } else if (MESSAGETYPEID_LASER_SENSOR == localBuffer[0]){
    forwardI2CMessage(LASER_SENSOR_I2C_ADDR);
  }
  nfcManager.handleI2CMessage(I2C_MESSAGE_MAX_SIZE, localBuffer);
  
  if(localBuffer[0] == 1){
    if('I' == commandSource){
      // only tell other xBee if the nextMode command came from I2C - fwds wireless signal
      instructXBeeModeChange(localBuffer[0]); 
    } 
    //Turn Off  
    laserController.turnOffAllLaser();
  } else if (localBuffer[0] == 3){
    if('I' == commandSource){
      // only tell other xBee if the nextMode command came from I2C - fwds wireless signal
      instructXBeeModeChange(localBuffer[0]); 
    }     
    //Turn On
    laserController.turnOnAllLaser();
  } 
  commandSource = ' ';

}

void forwardI2CMessage(int i2cAddr){
  Wire.beginTransmission(i2cAddr); //4 == ShelfLock
  Wire.write(localBuffer[0]);
  Wire.write(localBuffer[1]);
  Wire.write(localBuffer[2]);
  Wire.write(localBuffer[3]);
  Wire.endTransmission();
}


