#include <TinyWireS.h>

int lockPin = 1;
int lockState = HIGH;

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(uint8_t howMany)
{
  uint8_t mode = 0;
  if(TinyWireS.available() > 0){
    mode = TinyWireS.receive(); 
  }
  
  if(mode == 6){
    boolean isUnlock = TinyWireS.receive();    // receive byte as an integer
    if(isUnlock){
      lockState = HIGH;
      digitalWrite(lockPin, HIGH);
      delay(5000);
      digitalWrite(lockPin, LOW);
    }
  }
}

void setup()
{
  pinMode(lockPin, OUTPUT);
  TinyWireS.begin(4);
  TinyWireS.onReceive(receiveEvent); // register event
  
  digitalWrite(lockPin, HIGH);
  delay(1000);
  digitalWrite(lockPin, LOW);
  
}

void loop()
{
  TinyWireS_stop_check(); 
}

