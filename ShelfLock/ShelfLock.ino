#include <Wire.h>

int lockPin = 8;
int lockState = HIGH;

void setup()
{
  pinMode(lockPin, OUTPUT);
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  
  digitalWrite(lockPin, HIGH);
  delay(1000);
  digitalWrite(lockPin, LOW);
}

void loop()
{
  if(HIGH == lockState){
    delay(5000);
    lockState = LOW;
    digitalWrite(lockPin, lockState);
  }
  delay(1000);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  if(1==x){
    Serial.println(x);         // print the integer
    lockState = HIGH;
    digitalWrite(lockPin, lockState);
  }
}
