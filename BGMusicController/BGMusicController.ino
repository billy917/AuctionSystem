/* 
 * BGMusicController.ino
*/

#include "Playlist.h"

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

    playlist = new Playlist();
    fillDataSong();
    playlist->info();
    Serial.println ("Ready");

    playlist->playSong();
    pinMode(playlist->currentSongIndex + 2, OUTPUT);
} //end setup()

void loop() {
    while (verify != true) {
        playlist->status();
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
} //end loop()

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
} //end checkCode()

void fillDataSong(){
    playlist->songList[0] = new Song (186, "1725");
    playlist->songList[1] = new Song (172, "1804");
    playlist->songList[2] = new Song (217, "1936");
    playlist->songList[3] = new Song (223, "1935");
    playlist->songList[4] = new Song (170, "1867");
    playlist->songList[5] = new Song (132, "1875");
    playlist->songList[6] = new Song (147, "1880");
    playlist->songList[7] = new Song (170, "1787");
    playlist->songList[8] = new Song (208, "1756");
    playlist->songList[9] = new Song (136, "1942");
    playlist->songList[10] = new Song (137, "1892");

} //end fillDataSong()
