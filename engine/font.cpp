#include "font.h"
#include <algorithm>

FontManager* FontManager::instance = nullptr;

void FontManager::LoadFont(const char* fontPath, int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        printf("Could not init FreeType\n");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath, 0, &face)) {
        printf("Failed to load font\n");
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // Compute atlas size
    int atlasWidth = 0, atlasHeight = 0;
    int rowHeight = 0;
int maxBearingY = 0;
int maxBottom = 0;
const int padding = 2;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 32; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        maxBearingY = std::max(maxBearingY, face->glyph->bitmap_top);
        atlasWidth += face->glyph->bitmap.width + padding;
        int descender = face->glyph->bitmap.rows - face->glyph->bitmap_top;
        rowHeight = std::max<int>(rowHeight, face->glyph->bitmap.rows);
        maxBottom = std::max(maxBottom, descender);
    }
    atlasHeight = maxBearingY + maxBottom;
    this->maxBearingY = maxBearingY;

    // Create a single large texture
    glGenTextures(1, &fontTexture_1);
    glBindTexture(GL_TEXTURE_2D, fontTexture_1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    // Texture settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFinish(); //wtf opengl mention this before anything else
    

    int xOffset = 0;
    for (unsigned char c = 32; c < 127; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        //DEBUG_LOG("char - %c \n", c);
        
        FT_GlyphSlot g = face->glyph;

        glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        Character ch = {
            glm::vec2(xOffset / float(atlasWidth), 0),
            glm::vec2(g->bitmap.width, g->bitmap.rows),
            glm::vec2(g->bitmap_left, g->bitmap_top),
            float(g->advance.x >> 6)
        };

        Characters_font1[c] = ch;
        xOffset += g->bitmap.width;
    }

    atlas_width = atlasWidth;
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void FontManager::InitTextRendering() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * 400, NULL, GL_DYNAMIC_DRAW); // Reserve space for 1000 chars
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
