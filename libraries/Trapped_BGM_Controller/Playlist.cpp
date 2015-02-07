/*
 * Playlist.cpp
*/

#include "Playlist.h"

Playlist::Playlist () {
    fillDataSong();
    init();
}

void Playlist::init (){
    currentSongIndex = 0;
    currentSongLength = 0;
    currentSong = songList[0];
    Serial.println ("Playlist created successfully.");
}

void Playlist::fillDataSong() {
    for (int i = 0; i < TOTAL_SONGS; i++){
        songList[i] = new Song (180, String(i));
    }
}

void Playlist::info(){
    for (int i = 0; i < TOTAL_SONGS; i++){
        Serial.print ("Song# ");
        Serial.print (i);
        Serial.print ("; length: ");
        Serial.print (songList[i]->length);
        Serial.print ("; code: ");
        Serial.println (songList[i]->code);
    }
}

void Playlist::status(){
    Serial.print (currentSongLength);
    Serial.print (" - ");
    Serial.println (currentSong->length);

    if (currentSongLength >= currentSong->length){
        nextSong();
    } else{
        currentSongLength += 1;
    }
}

void Playlist::playSong(){
    Serial.print ("Playing Song # ");
    Serial.println (currentSongIndex);

}

void Playlist::nextSong() {
    currentSongIndex += 1;

    if (currentSongIndex > TOTAL_SONGS){
        currentSongIndex = 0;
    }

    currentSong = songList[currentSongIndex];
    currentSongLength = 0;

    playSong();

}

void Playlist::stopSong(){
    Serial.print ("Stopped playing # ");
    Serial.println (currentSongIndex);
    
}
