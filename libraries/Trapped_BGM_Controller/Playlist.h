/*
 * Playlist.h
 * a looped playlist containing a fixed amount of songs
 * NOTE: When playing is stopped, the song index does not reset
*/

#ifndef Playlist_h
#define Playlist_h

#include "Arduino.h"
#include "Song.h"

class Playlist{

    #define TOTAL_SONGS 11

    public:
        Playlist();
        Song *songList[TOTAL_SONGS];
        Song *currentSong;
        int currentSongIndex;
        int currentSongLength;
        void status();
        void info();
        void playSong();
        void nextSong();
        void stopSong();
        void update();
        bool hasExceedSongLength();

    protected:
        void init();
};

#endif
