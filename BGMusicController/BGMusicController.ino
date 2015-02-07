/* 
 * BGMusicController.ino
*/

#include "PlaylistPin.h"

// Initialize variables
int keypad = 0; // for keypad input
int failCount = 0; // for keeping track number of failed tries
bool verify = false;

Playlist *playlist;

void setup() {
    uint8_t i;
    for(i=2; i<=12; i++) pinMode(i, INPUT);
    for(i=2; i<=12; i++) digitalWrite(i, LOW);

    Serial.begin(9600); // opens serial port, sets data rate to 9600    

    playlist = new PlaylistPin();
    playlist->info();
    Serial.println ("Ready");

    playlist->playSong();
    pinMode(playlist->currentSongIndex + 2, OUTPUT);
}

void loop() {
    while (verify != true) {
        //playlist->status();
        if (playlist->currentSongLength >=
            playlist->currentSong->length) {
            
            playlist->stopSong();
            pinMode(playlist->currentSongIndex + 2, INPUT);

            playlist->nextSong();
            pinMode(playlist->currentSongIndex + 2, OUTPUT);

        } else {
            playlist->currentSongLength += 1;
        }

        verify = checkCode(); // check keypad input with song code
        delay(1000);
    }

    Serial.println ("Correct code");
}

bool checkCode() {
    if (Serial.available()>0){
        keypad = Serial.parseInt(); // read the incoming byte
        
        Serial.print ("I received: ");
        Serial.println (keypad);

        if (String(keypad).equals(playlist->currentSong->code)) {
            playlist->stopSong();
            pinMode (playlist->currentSongIndex + 2, INPUT);

            return true;

        }else {
            failCount += 1;
            Serial.print ("Wrong Code! The correct code is ");
            Serial.print (playlist->currentSong->code);
            Serial.print (" and ");
            Serial.print (3 - failCount);
            Serial.println (" tries left.");

            if (failCount >= 3) {
                failCount = 0;
                
                playlist->stopSong();
                pinMode (playlist->currentSongIndex + 2, INPUT);

                playlist->nextSong();
                pinMode (playlist->currentSongIndex + 2, OUTPUT);

            }
            return false;
        }
    }
}
