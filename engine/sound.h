//sound will have individual sound based stuff
    //load sound + meta data
//sound manager will initialise sound manager and keep track of sounds maps
    //add sounds to sound map
    //play sound
    //update sound
//sound system
    //soundSystem(manager)
    //init manager
    //will update the component level

#pragma once

#include <string>
#include <fstream>
#include <AL/al.h>

using namespace std;

//https://research.ncl.ac.uk/game/mastersdegree/workshops/audio/Sound%20Workshop.pdf
struct FMTCHUNK
{
    short           format;
    short           channels;
    unsigned long   srate;
    unsigned long   bps;
    short           balign;
    short           samp;
};

struct Sound {
    char*   data;
    float   length;
    int     bitRate;
    float   freqRate;
    int     size;
    int     channels;

    ALuint  buffer;

    Sound();
    ~Sound();

    void LoadFromWAV(string filename);
    void LoadWAVChunkInfo(ifstream &file, string &name, unsigned int &size);

    const char* GetData() const;
    int GetBitRate() const;
    float GetFrequency() const;
    int GetChannels() const;
    int GetSize() const;
    ALuint GetBuffer() const;

    ALenum GetOALFormat() const;
    float GetLength() const;
};