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

uint8_t xbeePayload[3] = { 0, 0, 0 };
XBeeAddress64 laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1);
XBeeAddress64 hackerAddr = XBeeAddress64(0x0013a200, 0x00000000);
ZBTxRequest laser2Tx = ZBTxRequest(laser2Addr, xbeePayload, sizeof(xbeePayload));
ZBTxRequest hackerTx = ZBTxRequest(hackerAddr, xbeePayload, sizeof(xbeePayload));

int laserControllerId, nfcManagerId = 0;
LaserController laserController(laserControllerId, true);
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
  for(int i=0; i<3; i++){
    pinMode(laserPins[i], OUTPUT);
    digitalWrite(laserPins[i], LOW);
    laserController.setLaserPin((laserControllerId * 3)+i, laserPins[i]);    
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
    handleMessage();;
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
    if(5 == xBeeDataBuffer[0] || 6 == xBeeDataBuffer[0] || MESSAGETYPEID_LASER_CONTROL == xBeeDataBuffer[0]
        || MESSAGETYPEID_LOCK == xBeeDataBuffer[0]
        || MESSAGETYPEID_BGM == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    } else if(MESSAGETYPEID_NFC_MANAGE == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
      xBeeDataBuffer[3] = rx.getData(3);
      xBeeDataBuffer[4] = rx.getData(3);
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
  xbee.send(laser2Tx);
}

void forwardI2CMessage(int i2cAddr){
  Wire.beginTransmission(i2cAddr); //4 == ShelfLock
  Wire.write(localBuffer[0]);
  Wire.write(localBuffer[1]);
  Wire.write(localBuffer[2]);
  Wire.endTransmission();
}

void handleMessage(){
  if(laserController.canHandleMessageType(localBuffer[0])){
      laserController.handleMessage(3, localBuffer);
  } else if(MESSAGETYPEID_LOCK == localBuffer[0]){
    forwardI2CMessage(LOCK_MANAGER_I2C_ADDR);
  } else if(MESSAGETYPEID_BGM == localBuffer[0]){
    forwardI2CMessage(MESSAGETYPEID_BGM);
  }
  nfcManager.handleI2CMessage(I2C_MESSAGE_MAX_SIZE, localBuffer);
  
  if('I' == commandSource){
    // only tell other xBee if the nextMode command came from I2C - fwds wireless signal
    instructXBeeModeChange(localBuffer[0]); 
  } 
  
  if(localBuffer[0] == 1){
    //Turn Off  
    disableLaserWire(0); 
    disableLaserWire(1);
    disableLaserWire(2);
  } else if (localBuffer[0] == 2){
    //Arm System
  } else if (localBuffer[0] == 3){
    //Turn On
    enableLaserWire(0);
    enableLaserWire(1);
    enableLaserWire(2);
  } else if(localBuffer[0] == 4){
    //Disable half laser 
    disableLaserWire(0); 
    disableLaserWire(1);
  } 
  commandSource = ' ';

}

void enableLaserWire(int index){
  /**
  Use case: turning on laserwire 
  Turns on laser, set servo location, and calibrate light sensor average readings + set sensor threadshold
  **/
  _turnOnLaser(index);
  _calibrateLightSensor(index);
  _turnOnLightSensor(index);
}

void pauseLaserWire(int index){
  /**
  Use case: pausing game
  Ignore sensor trigger and turn off laser
  **/  
  _turnOffLightSensor(index);
  _turnOffLaser(index);
}

void resumeLaserWire(int index){
   /**
   Use case: resuming game
   Turn on laser and reenable sensor trigger
   **/
   _turnOnLaser(index);
   _calibrateLightSensor(index);
   _turnOnLightSensor(index);
}

void disableLaserWire(int index){
  /**
  Use case: turn off game
  Turns off sensor trigger, turns laser off, and set servo back to init position
  **/
  _turnOffLightSensor(index);
  _turnOffLaser(index);
}

void _turnOnLaser(int index){
  /**
  Turn laser power on
  **/
  digitalWrite(laserPins[index], HIGH);
}

void _turnOffLaser(int index){
  /**
  Turn laser power off
  **/
  digitalWrite(laserPins[index], LOW);
}

void _calibrateLightSensor(int index){
  /**
  Take current light sensor reading and create a threadshold for light sensor
  **/ 
  ////// TO BE IMPLEMENTED //////
}

void _turnOnLightSensor(int index){
  /**
  Turn on light sensor, which would raise an event if sensor value is greater than predefined threashold
  **/  
  //////// TO BE IMPLEMENTED //////
}

void _turnOffLightSensor(int index){
  /**
  Turn off light sensor, which would ignore sensor value if it is greater than predefined threashold
  **/  
  /////// TO BE IMPLEMENTED //////
}

