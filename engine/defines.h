#ifndef DEFINES_H
#define DEFINES_H 

#include "glad/glad.h"

#include <glm/glm.hpp>
enum ResourceType {
    TEXTURE,
    SHADER ,
    SOUND_WAV,
    SOUND_STREAM,
    FONT,
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

struct Sprite {
    GLuint texture_id;
    glm::vec2 frame_size;
    glm::ivec2 sheet_size;
    int start_frame;
    int frame_count;
    float frame_duration;
    glm::vec2 pixel_offset;
};
#endif //DEFINES_H
