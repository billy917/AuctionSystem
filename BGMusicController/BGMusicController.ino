/* 
 * BGMusicController.ino
 * Receive commands from KEYPAD-LOCK CONTROLLER
*/

#include "Playlist.h"
#include "Constants.h"
#include "Wire.h"

#define PIN_OFFSET 2

/* Initialize variables */
uint8_t commandReceive = -1;
char currentPassword[5] = {};

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
    playlist->currentSong->code.toCharArray (currentPassword, 5);

    Serial.print ("Current password: ");
    Serial.println (currentPassword);
    
    /* delay one second for SafeKeyPad */
    delay(30);

    playlist->info();
    Serial.println ("Ready");

} //end setup()

void loop() {
    /* Display current song time */
    playlist->status();

    /* Manage the different commands received from UNO */
    if (commandReceive == MESSAGETYPEID_BGM_PLAY_SONG){
        
        playSong();
    
    }else if (commandReceive == MESSAGETYPEID_BGM_STOP_SONG){
        
        stopSong();

        /* Reset song index */
        playlist->currentSongIndex = 0;

    } else if ((commandReceive == MESSAGETYPEID_BGM_NEXT_SONG) |
                (playlist->currentSongLength >=
                playlist->currentSong->length)){

        nextSong();

    } else {
        if (playlist->currentSongLength > 0){
            playlist->currentSongLength += 1;
        }
    }

    /* Reset commandReceive */
    commandReceive = -1;

    delay(1000);

} //end loop()

/* I2C onReceive interrupt */
void Received (int noBytes){
    commandReceive = Wire.read();

    Serial.print ("Received command: ");
    Serial.println (commandReceive);

} //end Received()

/* I2C onRequest interrupt */
void Request(){
    wireWritePassword();

} //end Request()

void fillDataSong(){

    playlist->songList[0] = new Song (136, "1892");
    playlist->songList[1] = new Song (135, "1942");
    playlist->songList[2] = new Song (207, "1756");
    playlist->songList[3] = new Song (170, "1787");
    playlist->songList[4] = new Song (147, "1880");
    playlist->songList[5] = new Song (131, "1875");
    playlist->songList[6] = new Song (169, "1867");
    playlist->songList[7] = new Song (223, "1935");
    playlist->songList[8] = new Song (178, "1720");
    playlist->songList[9] = new Song (176, "1804");
    playlist->songList[10] = new Song (185, "1725");
   
    //playlist->songList[2] = new Song (217, "1936");

} //end fillDataSong()

void wireWritePassword(){

    int i;
    for (i = 0; i < 4; i++){
        Wire.write (currentPassword[i]);
    }

} //end wireWritePassword()

void playSong(){
    /* Send current song password to UNO */
    Wire.beginTransmission (KEYPAD_LOCK_I2C_ADDR);
    wireWritePassword();
    Wire.endTransmission();
    
    Serial.println ("Sent current password.");

    /* Play current song */
    playlist->playSong();
    pinMode (playlist->currentSongIndex + PIN_OFFSET, OUTPUT);
    playlist->currentSongLength = 1;

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
