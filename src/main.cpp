#include <stdio.h>
#include <stdint.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "grid.h"
#include "model.h"
#include "camera.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef uint32_t b32;

using namespace std;

#define WinWidth 1000
#define WinHeight 1000
#define GL_GREY .5, .5, .5, 1

glm::vec3 pointLightPositions[] =
{
	glm::vec3(2.3f, -1.6f, -3.0f),
	glm::vec3(-1.7f, 0.9f, 1.0f)
};

bool firstMouse = true;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 0.5f, 1.0f);

int main (int argc, char* argv[])
{

  SDL_Init(SDL_INIT_EVERYTHING);

  float lastFrame = static_cast<float>(SDL_GetTicks()) / 1000.0f;

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  Uint32 WindowFlags = SDL_WINDOW_HIDDEN|SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  SDL_Window *Window = SDL_CreateWindow("", 0, 0, 1000, 1000, WindowFlags);
  SDL_GLContext Context = SDL_GL_CreateContext(Window);
  SDL_GL_MakeCurrent(Window, Context);
  


   SDL_SetWindowSize(Window, WinWidth, WinWidth);
   SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
   SDL_SetWindowTitle(Window, "engine");
   SDL_ShowWindow(Window);

    camera GameCamera = {};
    initializeCamera(&GameCamera);

   gladLoadGLLoader(SDL_GL_GetProcAddress);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    glViewport(0, 0, WinWidth, WinHeight);

    shader lightingShader = shader("shaders/baselighting.vs", "shaders/baselighting.frag");
	
    Model ourModel = Model("assets/Nanosuit/nanosuit.obj");

    Model sponzaModel = Model("assets/Sponza/glTF/Sponza.gltf");

    shader gridshader = shader("shaders/grid.vert", "shaders/grid.frag");

    Grid grid = Grid(20, 20, &gridshader);
    grid.Init();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(Window, Context);
    ImGui_ImplOpenGL3_Init();

    
  
  b32 Running = 1;
  b32 FullScreen = 0;
  while (Running)
  {
   float currentFrame = static_cast<float>(SDL_GetTicks()) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    SDL_Event Event;
     while (SDL_PollEvent(&Event))
    {
        ImGui_ImplSDL2_ProcessEvent(&Event);

        if(Event.type == SDL_QUIT)
        {
                Running = false;
        }
        else if (Event.type == SDL_KEYDOWN)
        {
                float cameraSpeed = static_cast<float>(GameCamera.speed * deltaTime);
                switch (Event.key.keysym.sym)
                {                    
                    case SDLK_ESCAPE:
                        Running = false;
                        break;
                    case SDLK_f:
                        FullScreen = !FullScreen;
                        SDL_SetWindowFullscreen(Window, FullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                        break;
                    case SDLK_w:
                        GameCamera.position += cameraSpeed * GameCamera.cameraFront;
                        break;
                    case SDLK_s:
                        GameCamera.position -= cameraSpeed * GameCamera.cameraFront;
                        break;
                    case SDLK_a:
                        GameCamera.position -= glm::normalize(glm::cross(GameCamera.cameraFront, GameCamera.up)) * cameraSpeed;
                        break;
                    case SDLK_d:
                        GameCamera.position += glm::normalize(glm::cross(GameCamera.cameraFront,GameCamera.up)) * cameraSpeed;
                        break;
                }
            }
      else if(Event.type == SDL_MOUSEMOTION)
      {
                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) // Check if left button is pressed
                {
                    float xpos = static_cast<float>(Event.motion.x);
                    float ypos = static_cast<float>(Event.motion.y);

                    if (firstMouse) {
                        lastX = xpos;
                        lastY = ypos;
                        firstMouse = false;
                    }

                    float xoffset = xpos - lastX;
                    float yoffset = lastY - ypos;
                    lastX = xpos;
                    lastY = ypos;

                    float sensitivity = 0.1f;
                    xoffset *= sensitivity;
                    yoffset *= sensitivity;

                    GameCamera.yaw += xoffset;
                    GameCamera.pitch -= yoffset;

                    if (GameCamera.pitch > 89.0f)
                        GameCamera.pitch = 89.0f;
                    if (GameCamera.pitch < -89.0f)
                        GameCamera.pitch = -89.0f;

                    glm::vec3 front;
                    front.x = cos(glm::radians(GameCamera.yaw)) * cos(glm::radians(GameCamera.pitch));
                    front.y = sin(glm::radians(GameCamera.pitch));
                    front.z = sin(glm::radians(GameCamera.yaw)) * cos(glm::radians(GameCamera.pitch));
                    GameCamera.cameraFront = glm::normalize(front);
                }
          }
            else if (Event.type == SDL_MOUSEWHEEL)
        {
                GameCamera.fov -= static_cast<float>(Event.wheel.y);
                if (GameCamera.fov < 1.0f)
                    GameCamera.fov = 1.0f;
                if (GameCamera.fov > 45.0f)
                    GameCamera.fov = 45.0f;
                break;
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // create transformations
    glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view  = glm::lookAt(GameCamera.position,GameCamera.position + GameCamera.cameraFront, GameCamera.up);
    glm::mat4 projection = glm::perspective(glm::radians(GameCamera.fov), (float)WinWidth/ (float)WinHeight, 0.1f, 1000.0f);

    lightingShader.bind();
		
    //model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	

     glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgramId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
     glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgramId, "view"), 1, GL_FALSE, glm::value_ptr(view));
     glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgramId, "model"), 1, GL_FALSE, glm::value_ptr(model));		
    //
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    //
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    // glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].constant"), 1.0f);
    // glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].linear"), 0.009);
    // glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].quadratic"), 0.0032);
    //
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
    // glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    // glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].constant"), 1.0f);
    // glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].linear"), 0.009);
    // glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].quadratic"), 0.0032);

    ourModel.Draw(lightingShader);
    sponzaModel.Draw(lightingShader);

    grid.draw(view, projection);
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(Window);
  }


  lightingShader.unbind();
    gridshader.unbind();

  ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  SDL_GL_DeleteContext(Context);
  SDL_DestroyWindow(Window);
  SDL_Quit();
  return 0;
}

