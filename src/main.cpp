#include <stdio.h>
#include <stdint.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"


using namespace std;

typedef int32_t i32;
typedef uint32_t u32;
typedef uint32_t b32;

#define WinWidth 1000
#define WinHeight 1000

#define GL_GREY .5, .5, .5, 1

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 0.5f, 1.0f);

glm::vec3 cubeCenter = glm::vec3(0.0f, 0.0f, 0.0f);
float cubeSize = 1.0f;

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

   gladLoadGLLoader(SDL_GL_GetProcAddress);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

   glClearColor(GL_GREY);

   shader lightingShader("shaders/quad.vert", "shaders/quad.frag");
    
    float vertices[] = {
        -1.0f,  1.0f,  // Top-left
         1.0f,  1.0f,  // Top-right
         1.0f, -1.0f,  // Bottom-right
        -1.0f, -1.0f   // Bottom-left
    };

    unsigned int indices[] = {  // Note that we start from 0!
        0, 1, 2,  // First Triangle
        2, 3, 0   // Second Triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    // load and create a texture 
    // -------------------------
    //unsigned int texture1; //texture2;
    // texture 1
    // ---------
    //glGenTextures(1, &texture1);
    //glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    //unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    //if (data)
    //{
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else
    //{
    //   std::cout << "Failed to load texture" << std::endl;
    //}
    //stbi_image_free(data);

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
                float cameraSpeed = static_cast<float>(10.0 * deltaTime);
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
                        cameraPos += cameraSpeed * cameraFront;
                        break;
                    case SDLK_s:
                        cameraPos -= cameraSpeed * cameraFront;
                        break;
                    case SDLK_a:
                        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
                        break;
                    case SDLK_d:
                        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

                    yaw += xoffset;
                    pitch -= yoffset;

                    if (pitch > 89.0f)
                        pitch = 89.0f;
                    if (pitch < -89.0f)
                        pitch = -89.0f;

                    glm::vec3 front;
                    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                    front.y = sin(glm::radians(pitch));
                    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                    cameraFront = glm::normalize(front);
                }
          }
            else if (Event.type == SDL_MOUSEWHEEL)
        {
                fov -= static_cast<float>(Event.wheel.y);
                if (fov < 1.0f)
                    fov = 1.0f;
                if (fov > 45.0f)
                    fov = 45.0f;
                break;
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();

    glViewport(0, 0, WinWidth, WinHeight);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    lightingShader.bind();

    glUniform2f(glGetUniformLocation(lightingShader.shaderProgramId, "iResolution"), 1000.f, 1000.0f);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "iTime"), deltaTime);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(Window);
  }


  lightingShader.unbind();

  ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  SDL_GL_DeleteContext(Context);
  SDL_DestroyWindow(Window);
  SDL_Quit();
  return 0;
}

