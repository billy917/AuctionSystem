/*
 * Playlist.cpp
*/

#include "Playlist.h"

Playlist::Playlist () {
    init();
}

void Playlist::init (){
    currentSongIndex = 0;
    currentSongLength = 0;
    currentSong = songList[0];
    Serial.println ("Playlist created successfully.");
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
