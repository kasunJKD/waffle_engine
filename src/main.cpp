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
#include "model.h"
typedef int32_t i32;
typedef uint32_t u32;
typedef uint32_t b32;

using namespace std;

#define WinWidth 1000
#define WinHeight 1000
#define GL_GREY .5, .5, .5, 1

//GRID====================================
struct Grid {
    unsigned int width;
    unsigned int depth;
    shader* gridshade;
    std::vector<float> gridVertices;
    GLuint VAO, VBO;

    Grid(unsigned int w, unsigned int d, shader* s)
    {
        this->width = w;
        this->depth = d;
        this->gridshade = s;
    }

    void Init()
    {
        // Clear previous data
        gridVertices.clear();

        // Generate vertices for vertical lines
        for (unsigned int i = 0; i <= width; ++i) {
            float x = static_cast<float>(i);
            // Vertical line from (x, 0, 0) to (x, 0, depth)
            gridVertices.push_back(x);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(x);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(static_cast<float>(depth));
        }

        // Generate vertices for horizontal lines
        for (unsigned int j = 0; j <= depth; ++j) {
            float z = static_cast<float>(j);
            // Horizontal line from (0, 0, z) to (width, 0, z)
            gridVertices.push_back(0.0f);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(z);
            gridVertices.push_back(static_cast<float>(width));
            gridVertices.push_back(0.0f);
            gridVertices.push_back(z);
        }

        // Setup buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
   
    void draw(const glm::mat4& view, const glm::mat4& projection)
    {
        // Bind VAO and draw the grid
        // Use shader program
        gridshade->bind();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -2.0f, -2.0f));
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(glGetUniformLocation(gridshade->shaderProgramId, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
        glBindVertexArray(0);   
    }

     ~Grid()
        {
            // Cleanup OpenGL resources
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }
};


//GRID====================================END

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 pointLightPositions[] =
{
	glm::vec3(2.3f, -1.6f, -3.0f),
	glm::vec3(-1.7f, 0.9f, 1.0f)
};

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
    
    glViewport(0, 0, WinWidth, WinHeight);

    shader lightingShader = shader("shaders/baselighting.vs", "shaders/baselighting.frag");
	
    Model ourModel = Model("assets/Nanosuit/nanosuit.obj");

    shader gridshader = shader("shaders/grid.vert", "shaders/grid.frag");

    Grid grid = Grid(30, 30, &gridshader);
    grid.Init();
   //shader lightingShader("shaders/colors.vert", "shaders/colors.frag");
   //shader lightCubeShader("shaders/light_cube.vert", "shaders/light_cube.frag");

    // Vertex input data
    // float vertices[] = {
    // -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    //  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    //  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    //  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    // -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    // -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    //
    // -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //  0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    // -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    // -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //
    // -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    // -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    // -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    // -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    // -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    // -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    //
    //  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    //  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    //  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    //  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    //  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    //  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    //
    // -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    //  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    //  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    //  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    // -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    // -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    //
    // -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    //  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    //  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    //  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    // -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    // -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    // };
    //
    // unsigned int VBO, cubeVAO;
    // glGenVertexArrays(1, &cubeVAO);
    // glGenBuffers(1, &VBO);
    //
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //
    // glBindVertexArray(cubeVAO);
    //
    // // position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // normal attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    //
    // // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    // unsigned int lightCubeVAO;
    // glGenVertexArrays(1, &lightCubeVAO);
    // glBindVertexArray(lightCubeVAO);
    //
    // // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // create transformations
    glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view  = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WinWidth/ (float)WinHeight, 0.1f, 100.0f);

    lightingShader.bind();
		
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	

    glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgramId, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgramId, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgramId, "model"), 1, GL_FALSE, glm::value_ptr(model));		

    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].linear"), 0.009);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[0].quadratic"), 0.0032);

    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].linear"), 0.009);
    glUniform1f(glGetUniformLocation(lightingShader.shaderProgramId, "pointLights[1].quadratic"), 0.0032);

    ourModel.Draw(lightingShader);

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

