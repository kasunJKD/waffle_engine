#pragma once 
#include <string>
#include <unordered_map>
#include <glad/glad.h>

struct TextureManager {
    static TextureManager& Instance() {
        static TextureManager instance; // Single instance
        return instance;
    }

    std::unordered_map<std::string, GLuint> textures;
    GLuint loadTextureFromFile(
        const char* filename,
        const std::string& name,
        GLenum image_format = GL_RGB,
        GLint internal_format = GL_RGB,
        GLint level = 0,
        GLint border = 0
    );

    void clear();

    void BindTexture(const GLuint texID);

    void UnloadAllTextures();
    void flipstbionload();
private:
    TextureManager() {
            //flipstbionload();// Flip images on load
        }

    ~TextureManager() {
            UnloadAllTextures();
        }
    // Prevent copy and assignment
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

};

