#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <string>
#include <iostream>

struct Window
{
    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext glContext = nullptr;
    int width;
    int height;
    std::string title;

    // Initialize the window and OpenGL context
    bool init(const std::string& windowTitle, int windowWidth, int windowHeight);

    // Swap buffers and refresh the display
    void swapBuffers();

    // Clean up resources
    void cleanUp();

    // Handle window resizing and updating OpenGL viewport
    void resize(int newWidth, int newHeight);
};