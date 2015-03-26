/*
 * TripLaserController.ino
 * Makes a sound when laser is tripped
 * Connects to LaserDriver2 I2C
*/

#include "Arduino.h"
#include "Constants.h"
#include "Wire.h"

#define SOUND_BOARD_BEEP_PIN 2 //TODO

/* Initialize variables */
uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool receivedI2CMessage = false;

void setup(){
    
    /* Setup Serial monitor */
    Serial.begin (9600);
    
    /* Setup audio board pins */
    pinMode (SOUND_BOARD_BEEP_PIN, INPUT);
    digitalWrite (SOUND_BOARD_BEEP_PIN, LOW);

    /* Setup I2C Wire */
    Wire.begin (TRIP_LASER_I2C_ADDR);
    Wire.onReceive (Received);
    Wire.onRequest (Request);
    
    Serial.println ("System ready");
}

void loop(){
    if (receivedI2CMessage) handleI2CMessage();
}

void forwardI2CMessage (uint8_t i2cAddr){
    Wire.beginTransmission (i2cAddr);
    for (int i=0; i < I2C_MESSAGE_MAX_SIZE; i++){
        Wire.write (i2cDataBuffer[i]);
    }
    Wire.endTransmission();
} //end forwardI2CMessage()

/* I2C onReceive interrupt */
void Received (int noBytes){
    for (int i=0; i < noBytes && i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = Wire.read();
    }

    receivedI2CMessage = true;

} //end Received()

/* I2C onRequest interrupt */
void Request(){} //end Request()

void handleI2CMessage(){
    if (i2cDataBuffer[0] == MESSAGETYPEID_TRIP_LASER){
        manageTripLaser();
    }

    receivedI2CMessage = false;
}

void manageTripLaser(){
    if (i2cDataBuffer[1] == MESSAGETYPEID_TRIP_LASER_ON) {
    } else if (i2cDataBuffer[1] == MESSAGETYPEID_TRIP_LASER_OFF) {
    } else if (i2cDataBuffer[1] == MESSAGETYPEID_TRIP_LASER_BEEP) {
        pinMode (SOUND_BOARD_BEEP_PIN, OUTPUT);
        delay (1000);
        pinMode (SOUND_BOARD_BEEP_PIN, INPUT);
    } else {}
}
