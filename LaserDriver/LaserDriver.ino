//#include <Wire.h>
#include <Servo.h>
 
// Pin 13 has an LED connected on most Arduino boards.

int laserPins[3] = {4,8,12};
int servoPins[3][2] = {{3,5},{6,9},{10,11}};
int servoInactivePositions[3][2] = {{1,1},{1,1},{1,1}}; // {Top,Bottom}
int servoActivePositions[3][2] = {{45,90},{45,90},{45,90}}; // {Top,Bottom}
int servoCurrentPositions[3][2] = {{0,0},{0,0},{0,0}};

Servo servo1T;
Servo servo1B;
Servo servo2T;
Servo servo2B;
Servo servo3T;
Servo servo3B;
Servo servos[3][2] = {{servo1T,servo1B},{servo2T,servo2B},{servo3T,servo3B}};

unsigned long timer = 0;

// the setup routine runs once when you press reset:
void setup() {           

  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  for(int i=0; i<3; i++){
    pinMode(laserPins[i], OUTPUT);
    digitalWrite(laserPins[i], LOW);    
  }
  
  servo1T.attach(servoPins[0][0]);
  servo1B.attach(servoPins[0][1]);
  servo2T.attach(servoPins[1][0]);
  servo2B.attach(servoPins[1][1]);
  servo3T.attach(servoPins[2][0]);
  servo3B.attach(servoPins[2][1]);
  //Wire.begin(5);
  //Wire.onReceive(receiveEvent);
  
  resetServoPositions();

  timer = millis();   
}

boolean isEnabled = false;

// the loop routine runs over and over again forever:
void loop() {
  if(millis() - timer > 10000){
    Serial.println("waited 10s");
    // waited 10 secs    
    if(!isEnabled){
      enableLaserWire(0);
      enableLaserWire(1);
      enableLaserWire(2);
    } else {
      disableLaserWire(0); 
      disableLaserWire(1);
      disableLaserWire(2);
    }
    isEnabled = !isEnabled;
    timer = millis(); 
  }
}

void resetServoPositions(){
  for(int i=0; i<3; i++){
    _updateServoPosition(i,servoInactivePositions[i][0], servoInactivePositions[i][1]);
  }
}

void _updateServoPosition(int index, int topPosition, int bottomPosition){
  _moveServo(servos[index][0], servoCurrentPositions[index][0], topPosition);
  servoCurrentPositions[index][0] = topPosition;

  _moveServo(servos[index][1], servoCurrentPositions[index][1], bottomPosition);
  servoCurrentPositions[index][1] = bottomPosition;
}

void _moveServo(Servo servo, int currentPosition, int newPosition){
  int steps = 1;
  if (currentPosition > newPosition){
    steps = steps * -1; 
  }
  unsigned long servoTimer = millis();
  while( currentPosition != newPosition){
    if(millis() - servoTimer > 10){
      currentPosition = currentPosition + steps;
      servo.write(currentPosition);
      servoTimer = millis();
    }
  }
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
  _updateServoPosition(index, servoActivePositions[index][0], servoActivePositions[index][1]);
}

void _positionInactiveServo(int index){
  /**
  Set servo to inactive position
  **/
  _updateServoPosition(index, servoInactivePositions[index][0], servoInactivePositions[index][1]);
}

void _calibrateLightSensor(int index){
  /**
  Take current light sensor reading and create a threadshold for light sensor
  **/ 
  // !!!!!!!!!!! TO BE IMPLEMENTED !!!!!!!!!!!!!!!!
}

void _turnOnLightSensor(int index){
  /**
  Turn on light sensor, which would raise an event if sensor value is greater than predefined threashold
  **/  
  // !!!!!!!!!!! TO BE IMPLEMENTED !!!!!!!!!!!!!!!!
}

void _turnOffLightSensor(int index){
  /**
  Turn off light sensor, which would ignore sensor value if it is greater than predefined threashold
  **/  
  // !!!!!!!!!!! TO BE IMPLEMENTED !!!!!!!!!!!!!!!!
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
