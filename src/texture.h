#ifndef TEXTURE_H
#define TEXTURE_H

enum TextureType {
    TEST_TEXTURE
};

struct TextureHandle {
    unsigned int texture;
    TextureType type;
    int originalHeight;
    int originalWidth;
};


void SetTextureFiltering();

TextureHandle 
LoadTexture(const char* path, TextureType type);


#endif //TEXTURE_H
