#include "sound.h"

Sound::Sound()
{
    buffer = 0;
    bitRate = 0;
    freqRate = 0;
    length = 0;
    data = nullptr;
}

Sound::~Sound()
{
    delete data;
}

ALenum Sound::GetOALFormat()
{
    if(GetBitRate() == 16)
    {
        return GetChannels() == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    }
    else if(GetBitRate() == 8)
    {
        return GetChannels () == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8 ;
    }

    return AL_FORMAT_MONO8;

}

float Sound::GetLength () const
{
    return length;
}

void Sound::LoadFromWAV(string filename)
{
    ifstream file(filename.c_str(),ios::in|ios::binary);
    if(!file)
    {
        cout << "Failed to load WAV file '" << filename << "' !" << endl;
        return;
    }

    string chunkName;
    unsigned int chunkSize;

    while(!file.eof())
    {
        LoadWAVChunkInfo(file, chnunkName, chunkSize);

        if(chunkName == "RIFF")
        {
            file.seekg(4, ios_base::cur);
        }
        else if (chunkName == "fmt ")
        {
            FMTCHUNK fmt;

            file.read((char*)&fmt, sizeof(FMTCHUNK));

            bitRate = fmt.samp;
            freqRate = (float)fmt.srate;
            channels = fmt.channels;
        }
        else if(chunkName == "data")
        {
            size = chunkSize;
            data = new char[size];
            file.read((char*)&data, chunkSize);
        }
        else
        {
            file.seekg(chunkSize, ios_base::cur);
        }

    }

    file.close();
    lenght = (float)size / (channels*freqRate*(bitRate/8.0f))*1000.0f;
}

void Sound::LoadWAVChunkInfo(ifstream &file,
                            string &name,
                            unsigned int &size)
{
    char chunk[4];
    file.read((char*)&chunk, 4);
    file.read((char*)&size, 4);

    name = string(chunk, 4);
}

const char* Sound::GetData() const {
    return data;
}

int Sound::GetBitRate() const {
    return bitRate;
}

float Sound::GetFrequency() const {
    return freqRate;
}

int Sound::GetChannels() const {
    return channels;
}

ALuint Sound::GetBuffer() const {
    return buffer;
}





















