/*
 * PlaylistPin.cpp
*/

#include "PlaylistPin.h"

PlaylistPin::PlaylistPin(){
    PlaylistPin::fillDataSong();
    Playlist::init();

    //uint8_t i;
    //for(i=0; i<11; i++) pinMode(i, INPUT);
    //for(i=0; i<11; i++) digitalWrite(i, LOW);
}

void PlaylistPin::playSong(){
    Serial.print ("Playing Song # ");
    Serial.println (currentSongIndex);
    
   //pinMode(currentSongIndex + 2, OUTPUT);

}

void PlaylistPin::nextSong() {
    //stopSong();

    currentSongIndex += 1;
    if (currentSongIndex > TOTAL_SONGS){
        currentSongIndex = 2;
    }

    currentSong = songList[currentSongIndex];
    currentSongLength = 0;

    // start playing next song
    playSong();
}

void PlaylistPin::stopSong(){
    // stop current playing Song
    //pinMode (currentSongIndex + 2, INPUT);

    Serial.print ("Stopped playing # ");
    Serial.println (currentSongIndex);

}

void PlaylistPin::fillDataSong(){
    songList[0] = new Song (180, "1725");
    songList[1] = new Song (180, "1804");
    songList[2] = new Song (210, "1936");
    songList[3] = new Song (220, "1935");
    songList[4] = new Song (165, "1867");
    songList[5] = new Song (130, "1875");
    songList[6] = new Song (145, "1880");
    songList[7] = new Song (165, "1787");
    songList[8] = new Song (205, "1756");
    songList[9] = new Song (135, "1942");
    songList[10] = new Song (135, "1892");

}
