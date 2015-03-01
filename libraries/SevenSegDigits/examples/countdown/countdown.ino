#include <Digits.h>



int dataPin = 4;
int clockPin = 3;
int latchPin = 2;
int data[2];

Digits digits(dataPin, clockPin, latchPin);

void setup()
{
  
}

void loop()
{
  
  int count = 100;
  int val = 10; //this is for how many digits
 
  for( int i = 0; i<count; i++ )
  {
     int digit2 = i % val;
     int digit1 = ( i - digit2 ) / val;
     //Digits are added to the array in the same order as in the circuit
     data[0] = digit1;
     data[1] = digit2;

     digits.show( data, 2 );
     delay(1000);
  }
  
  
}

