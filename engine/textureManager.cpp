#include "textureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad/glad.h"
#include <iostream>
#include <string>


void TextureManager::flipstbionload()
{
    stbi_set_flip_vertically_on_load(true); // Flip images on load
}

GLuint TextureManager::loadTextureFromFile(
    const char* filename,
    const std::string& name,
    GLenum image_format,
    GLint internal_format,
    GLint level,
    GLint border
)
{
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

        if (!data || width == 0 || height == 0)
        {
            std::cerr<< "Failed to load texture: " << filename << std::endl;
            return 0;
        }

        GLuint gl_texID;
        glGenTextures(1, &gl_texID);
        glBindTexture(GL_TEXTURE_2D, gl_texID);

        GLenum format = image_format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, border, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        if (textures.find(name) != textures.end()) {
            std::cerr << "Texture with name '" << name << "' already exists!" << std::endl;
            return -1;
        }
        textures[name] = gl_texID;
        return gl_texID;
}

void TextureManager::clear()
{
    UnloadAllTextures();
}

void TextureManager::BindTexture(const GLuint texID)
{
        glBindTexture(GL_TEXTURE_2D, texID);
}

void TextureManager::UnloadAllTextures()
{
    for (auto it = textures.begin(); it != textures.end();++it)
    {
        glDeleteTextures(1, &it->second);
    }
    textures.clear();
}

