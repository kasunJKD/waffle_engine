#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <string>

struct Window
{
    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext glContext = nullptr;
    int width;
    int height;
    std::string title;

    bool init(const std::string& windowTitle, int windowWidth, int windowHeight);

    void swapBuffers();

    void cleanUp();

    void resize(int newWidth, int newHeight);
};
