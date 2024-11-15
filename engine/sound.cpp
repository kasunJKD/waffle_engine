#include "sound.h"
#include <iostream>

using namespace std;

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

ALenum Sound::GetOALFormat() const
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

void Sound::LoadFromWAV(string filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return;
    }

    std::cout << "Loading WAV file: " << filename << std::endl;

    std::string chunkName;
    unsigned int chunkSize;

    // Read through the file to process chunks
    while (file.tellg() != -1) {
        // Read the chunk name and size
        LoadWAVChunkInfo(file, chunkName, chunkSize);

        std::cout << "Chunk: " << chunkName << ", Size: " << chunkSize << std::endl;

        if (chunkName == "RIFF") {
            // Skip RIFF type (next 4 bytes)
            file.seekg(4, std::ios_base::cur);
        } else if (chunkName == "fmt ") {
            FMTCHUNK fmt;
            if (chunkSize < sizeof(FMTCHUNK)) {
                std::cerr << "Error: Invalid fmt chunk size." << std::endl;
                break;
            }

            file.read(reinterpret_cast<char*>(&fmt), sizeof(FMTCHUNK));
            bitRate = fmt.samp;
            freqRate = static_cast<float>(fmt.srate);
            channels = fmt.channels;

            std::cout << "Format Chunk: BitRate: " << bitRate
                      << ", Frequency: " << freqRate
                      << ", Channels: " << channels << std::endl;

            // Skip any extra bytes in the fmt chunk
            if (chunkSize > sizeof(FMTCHUNK)) {
                file.seekg(chunkSize - sizeof(FMTCHUNK), std::ios_base::cur);
            }
        } else if (chunkName == "data") {
            // Allocate memory and read the data chunk
            size = chunkSize;
            data = new char[size];
            file.read(data, size);

            std::cout << "Data Chunk Loaded. Size: " << size << " bytes." << std::endl;
        } else {
            // Unknown chunk; skip it
            file.seekg(chunkSize, std::ios_base::cur);
        }
    }

    file.close();

    // Calculate sound length
    if (channels > 0 && freqRate > 0 && bitRate > 0) {
        length = static_cast<float>(size) / (channels * freqRate * (bitRate / 8.0f)) * 1000.0f;
        std::cout << "Sound Length: " << length << " ms" << std::endl;
    } else {
        std::cerr << "Error: Failed to calculate sound length. Invalid WAV format." << std::endl;
    }
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

int Sound::GetSize() const {
    return size;
}





















