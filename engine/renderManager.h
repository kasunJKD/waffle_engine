#pragma once

#include "entityManager.h"
#include "sceneManager.h"
#include <vector>
#include <unordered_map>
#include <glad/glad.h>
#include "defines.h"

struct RenderManager
{
    //quad
    GLuint quadShader;
    GLuint quadVAO, quadVBO, quadEBO;

    //spritesheet
    GLuint spShader;
    GLuint spVAO, spVBO, spEBO;

    void init();

    std::unordered_map<SpriteID, Sprite> initializeSprites();
    std::pair<GLuint, GLuint> renderTileInit();
    void renderTile(
        int x,
        int y,
        const Sprite& sprite,
        GLuint textureID,
        int tileSize,
        float atlasWidth,
        float atlasHeight
    );

    void renderTile_P(
        const Sprite& sprite,
        GLuint textureID,
        float atlasWidth,
        float atlasHeight
    );
    void update(const EntityManager& entityManager, const SceneManager& sceneManager);
    void cleanUp();

    void renderLayer(
        const Layer& layer,
        int width,
        int height,
        const std::unordered_map<SpriteID, Sprite>& spriteMap,
        GLuint textureID,
        int tileSize,
        float atlasWidth,
        float atlasHeight
    );

    void renderLayers(
        const std::vector<Layer>& layers,
        int width,
        int height,
        const std::unordered_map<SpriteID, Sprite>& spriteMap,
        GLuint textureID,
        int tileSize,
        float atlasWidth,
        float atlasHeight
    );

    TiledMap parseTiledJSON(
        const std::string& filepath
    );

};

void quad_init(GLuint &quadVBO, GLuint &quadVAO, GLuint &quadEBO);
void quad_render(const TransformComponent& transform, const Shape_RectComponent& shape, GLuint shaderProgram);
