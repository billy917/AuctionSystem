//Coordinator xBee address - 40c04ef1
//I2C address: 2

#include <Wire.h>
#include <Servo.h>
#include <XBee.h>
#include "Constants.h"
#include "LaserController.h"
#include "NFCManager.h"

// Pin 13 has an LED connected on most Arduino boards.

int laserPins[3] = {4,8,12};
int servoPins[3][2] = {{3,5},{6,9},{10,11}};
int servoInactivePositions[3][2] = {{90,180},{90,160},{45,180}}; // {Top,Bottom}
int servoActivePositions[3][2] = {{0,90},{20,70},{-30,90}}; // {Top,Bottom}
int servoCurrentPositions[3][2] = {{0,0},{0,0},{0,0}};

Servo servo1T;
Servo servo1B;
Servo servo2T;
Servo servo2B;
Servo servo3T;
Servo servo3B;
Servo servos[3][2] = {{servo1T,servo1B},{servo2T,servo2B},{servo3T,servo3B}};

unsigned long timer = 0;

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
uint8_t commandData[] = {0,0};

uint8_t xbeePayload[3] = { 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));

XBeeAddress64 laser3Addr = XBeeAddress64(0x0013a200, 0x40c337e0);
ZBTxRequest laser3Tx = ZBTxRequest(laser3Addr, xbeePayload, sizeof(xbeePayload));

int laserControllerId, nfcManagerId = 2;
LaserController laserController(laserControllerId, false);
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
  for(int i=0; i<3; i++){
    pinMode(laserPins[i], OUTPUT);
    digitalWrite(laserPins[i],LOW);
    laserController.setLaserPin((laserControllerId * 3)+i, laserPins[i]);
  }
  
  resetServoPositions();

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
        || 6 == xBeeDataBuffer[0] 
        || MESSAGETYPEID_LASER_CONTROL == xBeeDataBuffer[0]
        || MESSAGETYPEID_LOCK == xBeeDataBuffer[0]
        || MESSAGETYPEID_BGM == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    } else if(MESSAGETYPEID_NFC_MANAGE == xBeeDataBuffer[0]
              || MESSAGETYPEID_CLOCK == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
      xBeeDataBuffer[3] = rx.getData(3);
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
  } else if(MESSAGETYPEID_CLOCK == localBuffer[0]){
    forwardI2CMessage(CLOCK_I2C_ADDR);
  } 
  nfcManager.handleI2CMessage(I2C_MESSAGE_MAX_SIZE, localBuffer);
  
  if(localBuffer[0] == 1){
    if('I' == commandSource){
      // only tell other xBee if the nextMode command came from I2C - fwds wireless signal
      instructXBeeModeChange(localBuffer[0]); 
    } 
    //Turn Off  
    disableLaserWire(0); 
    disableLaserWire(1);
    disableLaserWire(2);
  } else if (localBuffer[0] == 2){
    //Arm System
  } else if (localBuffer[0] == 3){
    if('I' == commandSource){
      // only tell other xBee if the nextMode command came from I2C - fwds wireless signal
      instructXBeeModeChange(localBuffer[0]); 
    } 
    
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

void forwardI2CMessage(int i2cAddr){
  Wire.beginTransmission(i2cAddr); //4 == ShelfLock
  Wire.write(localBuffer[0]);
  Wire.write(localBuffer[1]);
  Wire.write(localBuffer[2]);
  Wire.write(localBuffer[3]);
  Wire.endTransmission();
}

void moveLaser(int laserIndex, int movementDirection){
  if (laserIndex >= 0 && laserIndex < 3){
    if(movementDirection == 0){ 
      // move up - top servo
      //_moveServo(servos[laserIndex][0], servoCurrentPositions[laserIndex][0], servoCurrentPositions[laserIndex][0]+1);
      _moveServo(laserIndex, 0, servoCurrentPositions[laserIndex][0], servoCurrentPositions[laserIndex][0]+1, false);
    } else if(movementDirection == 1){
      // move down - top servo
      //_moveServo(servos[laserIndex][0], servoCurrentPositions[laserIndex][0], servoCurrentPositions[laserIndex][0]-1);
      _moveServo(laserIndex, 0, servoCurrentPositions[laserIndex][0], servoCurrentPositions[laserIndex][0]-1, false);
    } else if (movementDirection == 2){
      // move left - bottom servo
      //_moveServo(servos[laserIndex][1], servoCurrentPositions[laserIndex][1], servoCurrentPositions[laserIndex][1]+1);
      _moveServo(laserIndex, 1, servoCurrentPositions[laserIndex][1], servoCurrentPositions[laserIndex][1]+1, false);
    } else if (movementDirection == 3){
      // move  - bottom servo
      _moveServo(laserIndex, 1, servoCurrentPositions[laserIndex][1], servoCurrentPositions[laserIndex][1]-1, false);
    }
  }
}

void resetServoPositions(){
  for(int i=0; i<3; i++){ 
    _updateServoPosition(i,servoInactivePositions[i][0], servoInactivePositions[i][1], true);
  }
}

void _updateServoPosition(int index, int topPosition, int bottomPosition, boolean ignoreCurrentPosition){
    _moveServo(index, 0, servoCurrentPositions[index][0], topPosition, ignoreCurrentPosition);
    _moveServo(index, 1, servoCurrentPositions[index][1], bottomPosition, ignoreCurrentPosition);
}

void _moveServo(int laserIndex, int topBottomIndex, int currentPosition, int newPosition, boolean ignoreCurrentPosition){
  servos[laserIndex][topBottomIndex].attach(servoPins[laserIndex][topBottomIndex]);
  
  if(ignoreCurrentPosition){
    servos[laserIndex][topBottomIndex].write(newPosition);
  } else {
    int steps = 1;
    if (currentPosition > newPosition){
      steps = steps * -1; 
    }
    unsigned long servoTimer = millis();
  
    while( currentPosition != newPosition){
      if(millis() - servoTimer > 10){
        currentPosition = currentPosition + steps;
        servos[laserIndex][topBottomIndex].write(currentPosition);
        servoTimer = millis();
      }
    }
  }
  servos[laserIndex][topBottomIndex].detach();
  servoCurrentPositions[laserIndex][topBottomIndex] = newPosition;
}

void enableLaserWire(int index){
  /**
  Use case: turning on laserwire 
  Turns on laser, set servo location, and calibrate light sensor average readings + set sensor threadshold
  **/
  _turnOnLaser(index);
  _positionActiveServo(index);
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
  _positionInactiveServo(index);
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

void _positionActiveServo(int index){
  /**
  Set servo to active positions
  **/
  _updateServoPosition(index, servoActivePositions[index][0], servoActivePositions[index][1], false);
}

void _positionInactiveServo(int index){
  /**
  Set servo to inactive position
  **/
  _updateServoPosition(index, servoInactivePositions[index][0], servoInactivePositions[index][1], false);
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


/*
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
  }
  int x = Wire.read();    // receive byte as an integer
}
*/
