#include "renderManager.h"
#include "shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <json.hpp> // JSON library
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, glm::rotate, glm::scale
#include <glm/gtc/type_ptr.hpp>
using json = nlohmann::json;

void RenderManager::init()
{
    shader basic_shader_ = shader("engine/shaders/basic.vert", "engine/shaders/basic.frag");
    quadShader = basic_shader_.shaderProgramId;

    quad_init(quadVAO, quadVBO, quadEBO);
}

void RenderManager::update(const EntityManager& entityManager, const SceneManager& sceneManager) {
    glUseProgram(quadShader);
    glBindVertexArray(quadVAO);

    for (const entity_id entity : sceneManager.currentScene->entities) { // Use entity_id directly
        if (entityManager.metadata.at(entity).active == false) continue;  // Skip inactive entities

        // Query the components present for this entity
        uint32_t componentMask = entityManager.queryComponents(entity);

        // Check if the entity has both Transform and Shape_Rect components
        if ((componentMask & ComponentType::Transform) && (componentMask & ComponentType::Shape_Rect)) {
            // Fetch components
            const TransformComponent& transform = entityManager.transforms.at(entity);
            const Shape_RectComponent& shapeRect = entityManager.shape_rects.at(entity);

            // Render the entity's quad
            quad_render(transform, shapeRect, quadShader);
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void RenderManager::cleanUp()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
    glDeleteProgram(quadShader);
}

void quad_init(GLuint &VBO,GLuint &VAO,GLuint &EBO)
{
    const float RECTANGLE_VERTICES[] = {
        -1.0f, -1.0f,  // Bottom-left
         1.0f, -1.0f,  // Bottom-right
         1.0f,  1.0f,  // Top-right
        -1.0f,  1.0f   // Top-left
    };

    const unsigned int RECTANGLE_INDICES[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Generate and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RECTANGLE_VERTICES), RECTANGLE_VERTICES, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RECTANGLE_INDICES), RECTANGLE_INDICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void quad_render(const TransformComponent& transform, const Shape_RectComponent& shape, GLuint shaderProgram) {
    // Set color uniform
    GLint colorLoc = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(colorLoc, shape.color.r, shape.color.g, shape.color.b, shape.color.a);

    // Calculate model matrix for the quad
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(transform.position));
    model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(transform.scale * shape.size, 1.0f));

    // Set model matrix uniform
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f); // Assuming 800x600 window
GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Render the quad
    if (shape.filled) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    } else {
        glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
    }
}

// Creates a sample sprite map
std::unordered_map<SpriteID, Sprite> RenderManager::initializeSprites() {
    std::unordered_map<SpriteID, Sprite> sprites;
    // Suppose each tile is 32×32 in the atlas
    sprites[GROUND] = {{0,   0}, {32, 32}, 1};
    sprites[WATER]  = {{32,  0}, {32, 32}, 2};
    sprites[PLAYER] = {{32, 48}, {32, 48}, -1};
    return sprites;
}
// Initialize a 1x1 quad and load the shaders.
// Returns (VAO, shaderProgram) pair.
std::pair<GLuint, GLuint> RenderManager::renderTileInit()
{
    // Quad vertices in local 0..1 space
    float vertices[] = {
        // Positions    // Texture coords
        0.0f, 0.0f,     0.0f, 1.0f,  // bottom-left
        1.0f, 0.0f,     1.0f, 1.0f,  // bottom-right
        1.0f, 1.0f,     1.0f, 0.0f,  // top-right
        0.0f, 1.0f,     0.0f, 0.0f   // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    glGenVertexArrays(1, &spVAO);
    glGenBuffers(1, &spVBO);
    glGenBuffers(1, &spEBO);

    // Setup VAO
    glBindVertexArray(spVAO);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, spVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // aPos (location=0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // aTexCoord (location=1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Load/compile/link shaders (adjust paths as needed)
    shader basicShader("sandbox/shaders/sprite.vert", "sandbox/shaders/sprite.frag");
    spShader = basicShader.shaderProgramId;

    return { spVAO, spShader};
}
/**
 * Renders a single tile at tile grid coordinate (x,y),
 * with each tile = tileSize pixels in "world/pixel" space.
 * The 'model' matrix in the shader will move/scale the 1x1 quad to the proper position.
 */
void RenderManager::renderTile(
    int x,
    int y,
    const Sprite& sprite,
    GLuint textureID,
    int tileSize,
    float atlasWidth,
    float atlasHeight
) {
    if (!sprite.visible) return;

    // Build the model matrix so that 0..1 in local coords
    // becomes tileSize×tileSize, translated to (x*tileSize, y*tileSize).
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x * tileSize, y * tileSize, 0.0f));
    model = glm::scale(model, glm::vec3(tileSize, tileSize, 1.0f));

    // Calculate sub-rectangle in the atlas in [0..1] range
    float texLeft   = (float)sprite.atlasOffset.x / atlasWidth;
    float texRight  = (float)(sprite.atlasOffset.x + sprite.size.x) / atlasWidth;
    float texTop    = (float)sprite.atlasOffset.y / atlasHeight;
    float texBottom = (float)(sprite.atlasOffset.y + sprite.size.y) / atlasHeight;

    // Use shader
    glUseProgram(spShader);

    // Set model matrix
    GLint modelLoc = glGetUniformLocation(spShader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Set atlas coords
    GLint texCoordsLoc = glGetUniformLocation(spShader, "texCoords");
    float texCoords[] = { texLeft, texTop, texRight, texBottom };
    glUniform4fv(texCoordsLoc, 1, texCoords);

    // Bind the texture and set the sampler uniform
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint texture1Loc = glGetUniformLocation(spShader, "texture1");
    glUniform1i(texture1Loc, 0);  // "0" means GL_TEXTURE0

    // Draw the quad
    glBindVertexArray(spVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void RenderManager::renderTile_P(
    const Sprite& sprite,
    GLuint textureID,
    float atlasWidth,
    float atlasHeight
) {
    if (!sprite.visible) return;

    // Build the model matrix so that 0..1 in local coords
    // becomes tileSize×tileSize, translated to (x*tileSize, y*tileSize).
    glm::mat4 model(1.0f);
    model = glm::scale(model, glm::vec3(sprite.size.x, sprite.size.y, 1.0f));

    // Calculate sub-rectangle in the atlas in [0..1] range
    float texLeft   = (float)sprite.atlasOffset.x / atlasWidth;
    float texRight  = (float)(sprite.atlasOffset.x + sprite.size.x) / atlasWidth;
    float texTop    = (float)sprite.atlasOffset.y / atlasHeight;
    float texBottom = (float)(sprite.atlasOffset.y + sprite.size.y) / atlasHeight;

    // Use shader
    glUseProgram(spShader);

    // Set model matrix
    GLint modelLoc = glGetUniformLocation(spShader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Set atlas coords
    GLint texCoordsLoc = glGetUniformLocation(spShader, "texCoords");
    float texCoords[] = { texLeft, texTop, texRight, texBottom };
    glUniform4fv(texCoordsLoc, 1, texCoords);

    // Bind the texture and set the sampler uniform
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint texture1Loc = glGetUniformLocation(spShader, "texture1");
    glUniform1i(texture1Loc, 0);  // "0" means GL_TEXTURE0

    // Draw the quad
    glBindVertexArray(spVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * Renders one entire Layer. 
 *  - 'width' and 'height' are the number of tiles in that layer horizontally/vertically.
 *  - 'tileSize' is how many pixels each tile covers on screen.
 *  - 'atlasWidth'/'atlasHeight' are the atlas texture dimensions in pixels.
 */
void RenderManager::renderLayer(
    const Layer& layer,
    int width,
    int height,
    const std::unordered_map<SpriteID, Sprite>& spriteMap,
    GLuint textureID,
    int tileSize,
    float atlasWidth,
    float atlasHeight
) {
    if (!layer.visible) return;

    const std::vector<int>& data = layer.data;

    for (int tileY = 0; tileY < height; ++tileY) {
        for (int tileX = 0; tileX < width; ++tileX) {
            int tileGID = data[tileY * width + tileX];
            if (tileGID <= 0) continue;

            // Find the sprite that matches tileGID
            for (const auto& pair : spriteMap) {
                const Sprite& sprite = pair.second;
                if (sprite.gid == tileGID) {
                    renderTile(tileX, tileY, sprite,
                               textureID, tileSize,
                               atlasWidth, atlasHeight);
                    break;
                }
            }
        }
    }
}

/**
 * Renders all tile layers in order.
 * (Object layers are typically handled differently—e.g., you might draw objects with sprites 
 *  or bounding boxes. That is up to you.)
 */
void RenderManager::renderLayers(
    const std::vector<Layer>& layers,
    int width,
    int height,
    const std::unordered_map<SpriteID, Sprite>& spriteMap,
    GLuint textureID,
    int tileSize,
    float atlasWidth,
    float atlasHeight
) {
    for (const auto& layer : layers) {
        renderLayer(layer, width, height, spriteMap,
                    textureID, tileSize,
                    atlasWidth, atlasHeight);
    }
}

/**
 * Parses a Tiled JSON file for BOTH tile layers and object groups.
 * Returns a `TiledMap` structure containing:
 *  - `layers`: all tile layers
 *  - `objectGroups`: all object groups
 */
TiledMap RenderManager::parseTiledJSON(
    const std::string& filepath
){
    TiledMap tiledMap;  // <-- Our composite structure

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << "\n";
        return tiledMap;
    }

    json tiledJson;
    try {
        file >> tiledJson;
    } catch (const json::parse_error& e) {
        std::cerr << "Error: JSON parsing failed - " << e.what() << "\n";
        return tiledMap;
    }

    if (!tiledJson.contains("layers")) {
        std::cerr << "Error: JSON does not contain 'layers'.\n";
        return tiledMap;
    }

    // Extract each layer or object group
    for (const auto& layerJson : tiledJson["layers"]) 
    {
        // 1) Check if it's a TILE LAYER
        if (layerJson.contains("type") && layerJson["type"] == "tilelayer") {
            if (!layerJson.contains("data") ||
                !layerJson.contains("width") ||
                !layerJson.contains("height"))
            {
                std::cerr << "Error: Tile layer is missing data/width/height.\n";
                continue;
            }

            Layer layer;
            layer.visible = layerJson.value("visible", true);
            layer.name    = layerJson.value("name", "");
            layer.data    = layerJson["data"].get<std::vector<int>>();

            tiledMap.layers.push_back(layer);
        }
        // 2) Check if it's an OBJECT GROUP
        else if (layerJson.contains("type") && layerJson["type"] == "objectgroup") {
            ObjectGroup og;
            og.name    = layerJson.value("name", "");
            og.visible = layerJson.value("visible", true);

            // Parse the array of objects
            if (layerJson.contains("objects") && layerJson["objects"].is_array()) {
                for (auto& obj : layerJson["objects"]) {
                    TiledObject tobj;
                    tobj.name    = obj.value("name", "");
                    tobj.type    = obj.value("type", "");
                    tobj.x       = obj.value("x", 0.0f);
                    tobj.y       = obj.value("y", 0.0f);
                    tobj.width   = obj.value("width", 0.0f);
                    tobj.height  = obj.value("height", 0.0f);
                    tobj.visible = obj.value("visible", true);

                    og.objects.push_back(tobj);
                }
            }

            tiledMap.objectGroups.push_back(og);
        }
    }

    return tiledMap;
}

