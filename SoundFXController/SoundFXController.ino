/*
  SoundFXController.ino
  
*/

#include "Arduino.h"
#include "Constants.h"
#include "Wire.h"
#include "Timer.h"

#define PIN_OFFSET 3
#define NUM_TRACK 9
#define WARNING_TRACK 9

/* Initialize Variables */
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile uint8_t localBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool receivedI2CMessage = false;

int trackList[NUM_TRACK];

Timer t;
int playEvent;

int pin;

/* Debug use only */
//int inc = 0;

void setup(){
    /* Setting up laser sensor pins connecting to sound board */
    uint8_t i;
    for(i=0 + PIN_OFFSET; i<=NUM_TRACK + PIN_OFFSET; i++){
        pinMode(i, INPUT);
    }

    for(i=0 + PIN_OFFSET; i<=NUM_TRACK + PIN_OFFSET; i++){
        digitalWrite(i, LOW);
    }
    
    /* Setting up WARNING pin */
    pinMode (WARNING_TRACK, INPUT);
    digitalWrite (WARNING_TRACK, LOW);

    /* Setting up I2C Wire */
    Wire.begin(SOUNDFX_I2C_ADDR);
    Wire.onReceive (Received);
    Wire.onRequest (Request);

    Serial.begin(9600);

    /* Setting up music playlist */
    fillTrackData();
    
    Serial.println ("Ready");
}

void loop(){

    /*
    Serial.println ("Debug: playTrack");
    receivedI2CMessage = true;
    i2cDataBuffer[0] = MESSAGETYPEID_CLOCK;
    i2cDataBuffer[1] = MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT;

    if (inc > 10) inc = 0;
    i2cDataBuffer[3] = inc;
    */

    if (receivedI2CMessage){
        
        for (int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
            localBuffer[i] = i2cDataBuffer[i];
        }

        Serial.println ("Has received i2c message");

        if (localBuffer[0] == MESSAGETYPEID_CLOCK){
            if (localBuffer[1] == MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT){
                    
                    // get sensor id from i2cDataBuffer[4]
                    // play specified track

                    pin = trackList[localBuffer[3]-1] + PIN_OFFSET;

                    Serial.print ("Playing pin: ");
                    Serial.println (pin);

                    pinMode (pin, OUTPUT);
                    
                    delay (105);

                    Serial.print ("Stop playing pin: ");
                    Serial.println (pin);

                    pinMode (pin, INPUT);
                
            } else if (localBuffer[1] == 
                MESSAGETYPEID_CLOCK_PLAY_LAST_TRACK){
                
                pinMode (WARNING_TRACK, OUTPUT);

            } else if (localBuffer[1] ==
                MESSAGETYPEID_CLOCK_STOP_LAST_TRACK){

                pinMode (WARNING_TRACK, INPUT);
                
            }

        }
        receivedI2CMessage = false;
    }

    //t.update();
    //inc += 1;

}

/* I2C onReceive interrupt */
void Received (int noBytes){
    for (int i=0; i < noBytes && i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = Wire.read();
    }

    receivedI2CMessage = true;

} //end Received()

/* I2C onRequest interrupt */
void Request(){} //end Request()

void fillTrackData(){
    for (int i=0; i<NUM_TRACK; i++){
        trackList[i] = (NUM_TRACK - i);
    }
}

