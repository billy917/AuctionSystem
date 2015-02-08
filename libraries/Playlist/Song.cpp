/*
 * Song.cpp
*/

#include "Arduino.h"
#include "Song.h"

Song::Song(){
    code = "";
    length = 0;
}

Song::Song (int l, String c){
    code = c;
    length = l;
}

