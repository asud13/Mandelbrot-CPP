#include <SDL3/SDL.h>
#include <vector>
#include <cstdint> 
#include <iostream>

int main()
{
    // SDL 3 tutorial 
    // run with command: g++ fractal.cpp -IC:\SDL3\include -LC:\SDL3\lib -lSDL3 -o fractal.exe

    // initialize SDL  
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed to initialize: " << SDL_GetError() << "\n"; 
        return 1; 
    }

    int width = 900; 
    int height = 600; 

    // Create SDL window 
    SDL_Window* window = SDL_CreateWindow(
        "Mandelbrot Canvas", 
        width,
        height,
        SDL_WINDOW_RESIZABLE 
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n"; 
        SDL_Quit(); 
        return 1; 
    }

    // Create renderer 
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0); 
    
    // If given renderer is null print error recieved by SDL and mvoe on with your life 
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << "\n"; 
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create textures for mandelbrot set
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING,
        width, 
        height); 
    
    
    std::vector<uint32_t> pixels (width * height, 0); // dynamic array to hold pixel data

    // Now create event loop for handling running events 
    bool running = true;
    SDL_Event event; 

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false; // In the event that the close button action is triggered 
            }
        }
    

        // Set screen black 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // hex for black
        SDL_RenderClear(renderer); 

        // Draw something 
        SDL_FRect rect = {200.0f, 150.0f, 400.0f, 300.0f}; // initialize floats for SDL3, esp when initializing renderings 
        SDL_RenderLine (renderer, 100.0f, 100.0f, 400.0f, 500.0f); 

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);

        // Present 
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;       
}