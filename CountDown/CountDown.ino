//#include <Wire.h>
#include <TinyWireM.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <avr/power.h>
 
Adafruit_7segment disp = Adafruit_7segment();

uint8_t passiveState = LOW;
uint8_t activeState = HIGH;

int button1Pin = 1;

int startCount = 15;
int counter = 0;
int c = 0; 

void setup() {
  pinMode(button1Pin,INPUT);
  
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
  disp.begin(0x70);
  disp.setBrightness(3);
  resetCounter();
}

void loop() {
    if(0 == counter){
      for(;;){
        endDisplay();
        delay(2000);
      } 
    }
    
    uint8_t state1 = digitalRead(button1Pin);

    if(state1 == HIGH){
      resetCounter(); 
    }
    
    if(10 == c){
      c = 0;
      counter -= 1;
      updateDisplay(counter);
    }
    delay(100);
    c+=1;
}

void resetCounter(){
  counter = startCount;
  updateDisplay(counter);
}


void updateDisplay(int value) {
  disp.print(value);
  disp.writeDisplay();
}

void endDisplay(){
  disp.writeDigitNum(0, 0);
  disp.writeDigitNum(1, 0);
  disp.writeDigitNum(2, 0);
  disp.writeDigitNum(3, 0);
  disp.blinkRate(2);
  disp.writeDisplay();
}

