/*
 * PlaylistPin.h
*/

#ifndef PlaylistPin_h
#define PlaylistPin_h

#include "Playlist.h"

class PlaylistPin : public Playlist {

    public:
        PlaylistPin();
        void playSong();
        void nextSong();
        void stopSong();
       
    private:
        void fillDataSong();
    
};

#endif
