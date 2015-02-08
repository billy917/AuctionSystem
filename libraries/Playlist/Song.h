/* 
 * Song.h
*/

#ifndef Song_h
#define Song_h

#include "Arduino.h"

class Song{
    public:
        Song();
        Song (int l, String c);
        String code;
        int length;
};

#endif
