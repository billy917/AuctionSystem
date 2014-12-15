//#include <Wire.h>
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led1 = 6;
int led2 = 5;
int led3 = 4;

int ledPins[3] = {6,5,4};
int pattern[10][3] = {{HIGH,LOW,LOW}, {LOW,HIGH,LOW}, {LOW,LOW,HIGH}, {LOW,LOW,LOW}, {HIGH,LOW,LOW}, {HIGH,HIGH,LOW}, {HIGH,HIGH,HIGH}, {LOW,HIGH,HIGH},{LOW,LOW,HIGH},{LOW,LOW,LOW}};

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     
  pinMode(led3, OUTPUT);     
  
  //Wire.begin(5);
  //Wire.onReceive(receiveEvent);
}

// the loop routine runs over and over again forever:

int i = 0;

void loop() {
       
  for(int j=0;j<3;j++){
    digitalWrite(ledPins[j], pattern[i][j]);
  }
  delay(2000); 
  i++;

  if(10 == i){
    i = 0;
  } 
     
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
