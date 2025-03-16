#ifndef DEFINES_H
#define DEFINES_H 

#include "glad/glad.h"
enum ResourceType {
    TEXTURE,
    SHADER ,
    SOUND_WAV,
    SOUND_STREAM
};

union dataTypeResource {
    GLuint i;
    void *v;
};


struct Resource {
    ResourceType type;
    const char* path;
    const char* name;
    dataTypeResource data;
    size_t size;
};

#endif //DEFINES_H
