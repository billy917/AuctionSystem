/*
 * BGMusicController.ino
 * Receive commands from KEYPAD-LOCK CONTROLLER
*/

#include "Playlist.h"
#include "Constants.h"
#include "Wire.h"

#define PIN_OFFSET 2

/* Initialize variables */
uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool receivedI2CMessage = false;
char currentPassword[5] = {};

bool isPlaying = false;

Playlist *playlist;

void setup() {
    /* Setting up pins connecting to sound board */
    uint8_t i;
    for(i=0 + PIN_OFFSET; i<=10 + PIN_OFFSET; i++) pinMode(i, INPUT);
    for(i=0 + PIN_OFFSET; i<=10 + PIN_OFFSET; i++) digitalWrite(i, LOW);

    /* Setting up I2C Wire */
    Wire.begin(BGM_I2C_ADDR); // this BGM Controller I2C port
    Wire.onReceive (Received);
    Wire.onRequest (Request);

    Serial.begin(9600);

    /* Setting up music playlist */
    playlist = new Playlist();
    fillDataSong();
    playlist->currentSong = playlist->songList[0];
    playlist->currentSong->code.toCharArray (currentPassword, 5);

    Serial.print ("Current password: ");
    Serial.println (currentPassword);

    /* delay for SafeKeyPad */
    delay(30);

    playlist->info();
    Serial.println ("Ready");

} //end setup()

void loop() {
    /* Display current song time */
    playlist->status();

    if (playlist->hasExceedSongLength()){
        Serial.println ("Exceed song length");
        nextSong();
        isPlaying = true;
    }

    if (receivedI2CMessage){
        Serial.println ("Has received i2c message");

        if (i2cDataBuffer[0] == MESSAGETYPEID_BGM){
            /* Manage the different commands received from UNO */
            if (i2cDataBuffer[1] == MESSAGETYPEID_BGM_PLAY_SONG){
                Serial.println ("MSGID: PLAY_SONG");

                playSong();
                isPlaying = true;

            } else if (i2cDataBuffer[1] == MESSAGETYPEID_BGM_STOP_SONG){
                Serial.println ("MSGID: STOP_SONG");

                stopSong();
                isPlaying = false;

            } else if (i2cDataBuffer[1] == MESSAGETYPEID_BGM_NEXT_SONG){
                Serial.println ("MSGID: NEXT_SONG");

                nextSong();
                isPlaying = true;

            } else if (i2cDataBuffer[1] == MESSAGETYPEID_BGM_UPDATE){
                Serial.println ("MSGID: UPDATE");
                playlist->currentSongIndex = 0;

                Wire.beginTransmission (KEYPAD_LOCK_I2C_ADDR);
                Wire.write (MESSAGETYPEID_KEYPAD_LOCK);
                Wire.write (MESSAGETYPEID_KEYPAD_LOCK_RESET);
                Wire.endTransmission();

                playSong();
                isPlaying = true;

            } else {}

        } else if (i2cDataBuffer[0] == MESSAGETYPEID_NFC_MANAGE){
            forwardI2CMessage (KEYPAD_LOCK_I2C_ADDR);

        } else {}

        /* Reset receivedI2CMessage */
        Serial.println ("Reset I2C state");
        receivedI2CMessage = false;

        clearI2CBuffer();

    }

    if (isPlaying){
        playlist->update();
    }

    delay(1000);

} //end loop()

void forwardI2CMessage (uint8_t i2cAddr){
    Wire.beginTransmission (i2cAddr);
    for (int i=0; i < I2C_MESSAGE_MAX_SIZE; i++){
        Wire.write (i2cDataBuffer[i]);
    }
    Wire.endTransmission();
}

/* I2C onReceive interrupt */
void Received (int noBytes){
    for (int i=0; i < noBytes && i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = Wire.read();
        //Serial.println(i2cDataBuffer[i]);
    }

    receivedI2CMessage = true;
    //Serial.print ("Received command: ");
    //Serial.println (receivedI2CMessage);

} //end Received()

/* I2C onRequest interrupt */
void Request(){
    //wireWritePassword();

} //end Request()

void fillDataSong(){
    /* song_index 0 = PIN D2 = Song 10 */
    playlist->songList[10] = new Song (185, "1725"); // T00
    playlist->songList[9] = new Song (171, "1804");
    playlist->songList[8] = new Song (178, "1720");
    playlist->songList[7] = new Song (223, "1935");
    playlist->songList[6] = new Song (169, "1867");
    playlist->songList[5] = new Song (132, "1875");
    playlist->songList[4] = new Song (147, "1880");
    playlist->songList[3] = new Song (170, "1787");
    playlist->songList[2] = new Song (207, "1756");
    playlist->songList[1] = new Song (135, "1942");
    playlist->songList[0] = new Song (137, "1892"); // T10

    //playlist->songList[2] = new Song (217, "1936");

} //end fillDataSong()

void wireWritePassword(){

    Wire.write (MESSAGETYPEID_KEYPAD_LOCK);
    Wire.write (MESSAGETYPEID_KEYPAD_LOCK_GET_PASSWORD);

    int i;
    for (i = 0; i < 4; i++){
        Wire.write (currentPassword[i]);
    }

} //end wireWritePassword()

void clearI2CBuffer(){
    for (int i=0; i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = 0;
    }
}

void playSong(){
    /* Send current song password to RX */
    Wire.beginTransmission (KEYPAD_LOCK_I2C_ADDR);
    wireWritePassword();
    Wire.endTransmission();

    Serial.print ("Sent current password: ");
    Serial.println (currentPassword);

    /* Play current song */
    playlist->playSong();
    pinMode (playlist->currentSongIndex + PIN_OFFSET, OUTPUT);

} //end playSong()

void stopSong(){
    /* Stop playing current song. */
    playlist->stopSong();
    pinMode (playlist->currentSongIndex + PIN_OFFSET, INPUT);

} //end stopSong()

void nextSong(){

    stopSong();

    /* Increment song index */
    playlist->nextSong();

    /* Get new song password */
    playlist->currentSong->code.toCharArray (currentPassword, 5);

    playSong();

} //end nextSong()
