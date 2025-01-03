#pragma once

#include "stb_image.h"
#include <glad/glad.h>
#include <vector>
#include <algorithm>

class TextureManager
{
public:
    static TextureManager* Inst()
    {
        if (!m_inst)
        {
            m_inst = new TextureManager();
        }
        return m_inst;
    }

    ~TextureManager()
    {
        UnloadAllTextures();
    }

    GLuint LoadTexture(const char* filename, GLenum image_format = GL_RGB, GLint internal_format = GL_RGB, GLint level = 0, GLint border = 0)
    {
    stbi_set_flip_vertically_on_load(true); // Flip images on load
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

        if (!data || width == 0 || height == 0)
        {
            std::cerr << "Failed to load texture: " << filename << std::endl;
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

        m_texID.push_back(gl_texID);
        return gl_texID;
    }

    void UnloadTexture(const GLuint texID)
    {
        auto it = std::find(m_texID.begin(), m_texID.end(), texID);
        if (it != m_texID.end())
        {
            glDeleteTextures(1, &texID);
            m_texID.erase(it);
        }
    }

    void BindTexture(const GLuint texID)
    {
        glBindTexture(GL_TEXTURE_2D, texID);
    }

    void UnloadAllTextures()
    {
        for (GLuint texID : m_texID)
        {
            glDeleteTextures(1, &texID);
        }
        m_texID.clear();
    }

private:
    TextureManager() {}
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    static TextureManager* m_inst;
    std::vector<GLuint> m_texID;
};
