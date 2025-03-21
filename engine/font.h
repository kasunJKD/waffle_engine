#ifndef FONT_H
#define FONT_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <iostream>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H 

struct Character {
	glm::vec2 uv;      // Texture coordinates in atlas
	glm::vec2 size;    // Glyph size
	glm::vec2 bearing; // Offset from baseline
	float advance;     // Horizontal advance
};


struct FontManager {
public:
    std::map<char, Character> Characters_font1;
    GLuint fontTexture_1;
    GLuint VAO;
    GLuint VBO;
    int atlas_width;
    float maxBearingY = 0;

    // Get the global instance
    static FontManager* GetInstance() {
        if (instance == nullptr) {
            instance = new FontManager();
        }
        return instance;
    }

    // Cleanup memory
    static void DestroyInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

    void LoadFont(const char* fontPath, int fontSize); // Define this later
    void InitTextRendering();

    FontManager() { std::cout << "FontManager Created!" << std::endl; }
    ~FontManager() { std::cout << "FontManager Destroyed!" << std::endl; }

    // Prevent copy/assignment
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    static FontManager* instance; // Static pointer (heap allocation)
};

#endif //FONT_H
