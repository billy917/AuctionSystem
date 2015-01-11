//Coordinator xBee address - 40c04edf
//I2C address: 1

#include <Wire.h>
#include <Servo.h>
#include <XBee.h>
 
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

// the setup routine runs once when you press reset:
void setup() {           

  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  for(int i=0; i<3; i++){
    pinMode(laserPins[i], OUTPUT);
    digitalWrite(laserPins[i], LOW);    
  }
  
  /*
  servo1T.attach(servoPins[0][0]);
  servo1B.attach(servoPins[0][1]);
  servo2T.attach(servoPins[1][0]);
  servo2B.attach(servoPins[1][1]);
  servo3T.attach(servoPins[2][0]);
  servo3B.attach(servoPins[2][1]);
  */
  //Wire.begin(5);
  //Wire.onReceive(receiveEvent);
  
  resetServoPositions();

  Wire.begin(1);
  Wire.onReceive(receiveEvent);
  xbee.begin(Serial);   
}

volatile int nextMode = 0;

void loop() {
  if(nextMode == 0){
    handleXBeeMsg();
  }
  handleCommands();
}

void receiveEvent(int howMany){
  while(Wire.available()){
    nextMode = Wire.read(); 
  }
}

void handleXBeeMsg(){
  xbee.readPacket();
  if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);
    nextMode = rx.getData(0);
    if(5 == nextMode){
      commandData[0] = rx.getData(1);
      commandData[1] = rx.getData(2);
    }
  } 
}

void handleCommands(){
  if(nextMode != 0){
    if(nextMode == 1){
      //Turn Off  
      disableLaserWire(0); 
      disableLaserWire(1);
      disableLaserWire(2);
    } else if (nextMode == 2){
      //Arm System
    } else if (nextMode == 3){
      //Turn On
      enableLaserWire(0);
      enableLaserWire(1);
      enableLaserWire(2);
    } else if(nextMode == 4){
      //Disable half laser 
      disableLaserWire(0); 
      disableLaserWire(1);
    } else if(nextMode == 5){
      //Ad-Hoc reposition laser
      //laserIndex = ; // possible value: 0, 1, 2
      //movementDirection =  //0 = up, 1 = down, 2 = left, 3 = right
      moveLaser(commandData[0], commandData[1]);
    }
    nextMode = 0;
  } 
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
