#include "engine.h" // IWYU pragma: keep
#include "entity.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate
#include <glm/gtc/type_ptr.hpp>
#include "resourceManager.h"
#include <iostream>

#ifdef DEBUG_ENABLED
#include "debug.h"
#endif

#include <cstdio>
#include <cstdlib>

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540
#define GAME_WIDTH 512
#define GAME_HEIGTH 288

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; 

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}
void CheckGLError(const std::string& message) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error (" << message << "): " << err << std::endl;
    }
}

struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec2 position; // Camera position in the world
    float zoom;         // Camera zoom level
    float width;        // camera width = 512
    float height;       // camera height = 288
};

GLuint quadShaderProgram;

GLuint quadVAO, quadVBO;

// void UpdateCamera(Camera &camera, int screenWidth, int screenHeight) {
//     camera.projection = glm::ortho(0.0f, static_cast<float>(screenWidth), 
//                                    static_cast<float>(screenHeight), 0.0f, -10.0f, 10.0f);
//     camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(-camera.position.x, -camera.position.y, -5.0f));
// }
void UpdateCamera(Camera &camera)
{
    // Orthographic 2D projection that is 512x288 in size
    // (left=0, right=512, top=0, bottom=288, near=-1, far=+1)
    camera.projection = glm::ortho(
        0.0f, camera.width,    // left, right
        camera.height, 0.0f,   // bottom, top  (note the flip if you like Y up vs. Y down)
        -1.0f, 1.0f
    );

    // The view is just a translation that offsets by the camera’s position in the world
    camera.view = glm::translate(glm::mat4(1.0f), 
                                 glm::vec3(-camera.position.x, -camera.position.y, 0.0f));
}

void CreateFramebuffer(GLuint &fbo, GLuint &texture) {
    glGenFramebuffers(1, &fbo);
CheckGLError("glgenframebuffers");
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

CheckGLError("glbindframebuffers");
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
CheckGLError("glframebuffers");
unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

   // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        std::cerr << "Framebuffer not complete! Status: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind 
}

void InitFrameBuffer(World &world1, World &world2) {
    CreateFramebuffer(world1.fbo, world1.fboTexture);
    CreateFramebuffer(world2.fbo, world2.fboTexture);
}

void RenderWorldToFBO(World &world) {
    glBindFramebuffer(GL_FRAMEBUFFER, world.fbo);
CheckGLError("glbindframebuffers");
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render the background image into this FBO. 
    glUseProgram(quadShaderProgram);

    // For a full-screen pass from -1..1, you can use identity or your existing
    // camera projection if it matches. Commonly you'd use an identity for the quad:
    glm::mat4 identity(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(quadShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(glGetUniformLocation(quadShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identity));

    glBindVertexArray(quadVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, world.texture);
    glUniform1i(glGetUniformLocation(quadShaderProgram, "screenTexture"), 0);

    // IMPORTANT: match your vertex data (6 verts) with your draw call:
    glDrawArrays(GL_TRIANGLES, 0, 6);  

    glBindVertexArray(0);

    // done drawing into the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Render your world-specific objects here
    // Example: RenderWorldObjects(world.worldnumber);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0); // Switch back to default framebuffer
}

void InitQuad() {
 float quadVertices[] = {
        // position     // texCoord
         0.0f,    0.0f,    0.0f, 0.0f,
         3072.0f, 0.0f,    1.0f, 0.0f,
         3072.0f, 1728.0f, 1.0f, 1.0f,

         0.0f,    0.0f,    0.0f, 0.0f,
         3072.0f, 1728.0f, 1.0f, 1.0f,
         0.0f,    1728.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
CheckGLError("glGenVertexArrays");
    glGenBuffers(1, &quadVBO);
CheckGLError("glGenBuffers");

    glBindVertexArray(quadVAO);
CheckGLError("glbindvertexarray");

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
CheckGLError("glbindbuffer");
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position attribute (2D quad)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void RenderWorld(GLuint bigWorldTexture, const Camera &camera, GLuint shaderProgram)
{
    glUseProgram(shaderProgram);

    // Pass in projection & view to the shader
    GLint locProjection = glGetUniformLocation(shaderProgram, "projection");
    GLint locView       = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(locProjection, 1, GL_FALSE, glm::value_ptr(camera.projection));
    glUniformMatrix4fv(locView,       1, GL_FALSE, glm::value_ptr(camera.view));

    // If your shader also expects a model matrix, you can pass an identity or skip it
    // because we already put the real-world positions in the vertex data directly.

    // Bind the big texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bigWorldTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

    // Draw the big quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void RenderTexture(GLuint texture,  const Camera &camera) {
    glUseProgram(quadShaderProgram);
CheckGLError("gluseprogram");

    glUniformMatrix4fv(glGetUniformLocation(quadShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
    glUniformMatrix4fv(glGetUniformLocation(quadShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera.view));

    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
CheckGLError("glactivetexture");

    glBindTexture(GL_TEXTURE_2D, texture);
CheckGLError("glBindTexture");
    glUniform1i(glGetUniformLocation(quadShaderProgram, "screenTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


int main() {
    Window window;
    InputManager inputManager;

// Initialize the camera
    Camera camera;
    camera.position = glm::vec2(0.0f, 0.0f);
    camera.width = 512.0f;
    camera.height = 288.0f;
    UpdateCamera(camera);
    
    // Initialize the window
    if (!window.init("Sandbox", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
        DEBUG_ERROR("Failed to initialize window.");
        return -1;
    }
    
    size_t arenaSize = 10 * (1024 * 1024); // For example, 1 MB.
    MEM::MemoryArena* arena = MEM::arena_create(arenaSize);
    if (!arena) {
        DEBUG_ERROR("Failed to create MemoryArena.");
        return 1;
    }

    ResourceManager* r_manager = createResourceManager(arenaSize);
    Resource* spritesheet = load(r_manager, "assets/tiled/testSpritesheet.png", nullptr, TEXTURE);
    Resource* world1_background = load(r_manager, "assets/test_game/testWorld1.png", nullptr, TEXTURE);
    Resource* world2_background = load(r_manager, "assets/test_game/testWorld2.png", nullptr, TEXTURE);
    Resource* quad_shader_resource = load(r_manager, "sandbox/shaders/quad.vert", "sandbox/shaders/quad.frag", SHADER);
    quadShaderProgram = quad_shader_resource->data.i;
    if (!spritesheet)
    {
        DEBUG_ERROR("spritesheet error");
        return -1;
    }

    //######test world rendering##########
    //#############################
    InitQuad();

    int8_t active_world = 1;
    World world1, world2;
    world1.texture = world1_background->data.i;
    world1.worldnumber = 1;
    world2.texture = world2_background->data.i;
    world2.worldnumber = 2;
    InitFrameBuffer(world1, world2);

    inputManager.init();

    bool isRunning = true;
    while (isRunning) {
         uint32_t frameStart = SDL_GetTicks();
        //float dt = calculateDeltaTime();
         inputManager.update(isRunning);
        
        //###test rendering###########
 // 1) Render each world’s background into its FBO:
    RenderWorldToFBO(world1);
    RenderWorldToFBO(world2);

    // 2) Now draw whichever world is active onto the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    World &activeWorld = (active_world == 1) ? world2 : world1;

    RenderWorld(activeWorld.texture, camera, quadShaderProgram);

        window.swapBuffers();

// Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    
    MEM::arena_print_stats(arena);

    // In a real application, you might reset the arena instead of destroying it.
    free(arena->base);
    free(arena);
    destroyResourceManager(r_manager);
    window.cleanUp();
    return 0;
}
