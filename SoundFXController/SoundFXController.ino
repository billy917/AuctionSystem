/*
  SoundFXController.ino
  
*/

#include "FXPlaylist.h"
#include "Arduino.h"
#include "Constants.h"
#include "Wire.h"

#define PIN_OFFSET 2
#define NUM_TRACK 9

/* Initialize Variables */
uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool receivedI2CMessage = false;

int trackList[NUM_TRACK];

void setup(){
    /* Setting up pins connecting to sound board */
    uint8_t i;
    for(i=0 + PIN_OFFSET; i<=NUM_TRACK + PIN_OFFSET; i++){
        pinMode(i, INPUT);
    }

    for(i=0 + PIN_OFFSET; i<=NUM_TRACK + PIN_OFFSET; i++){
        digitalWrite(i, LOW);
    }

    /* Setting up I2C Wire */
    //Wire.begin(SOUNDFX_I2C_ADDR);
    //Wire.onReceive (Received);
    //Wire.onRequest (Request);

    Serial.begin(9600);

    /* Setting up music playlist */
    fillTrackData();
    
    Serial.println ("Ready");
}

void loop(){
    
    if (receivedI2CMessage){
        Serial.println ("Has received i2c message");


        if (i2cDataBuffer[0] == MESSAGETYPEID_CLOCK){
            if (i2cDataBuffer[1] == MESSAGETYPEID_CLOCK_MODIFY){
                if (i2cDataBuffer[2] ==
                    MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT){
                    
                    // get sensor id from i2cDataBuffer[4]
                    // play specified track

                    int pin = trackList[i2cDataBuffer[4]];
                    pinMode (pin + PIN_OFFSET, OUTPUT);

                    delay (1500);

                    pinMode (pin + PIN_OFFSET, INPUT);
                }
                
            }
        }
    
    }
    
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

void clearI2CBuffer(){
    for (int i=0; i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = 0;
    }
}
