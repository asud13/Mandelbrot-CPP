#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include <iostream>

int main() {

    // SDL INIT VIDEO: 
    // Initializes SDL video subsystem where you create windows, renderers and graphic displays 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // Sample width and height that are adjustable by SDL_WINDO_RESIZABLE 
    int width = 900;
    int height = 600;

    SDL_Window* window = SDL_CreateWindow("Mandelbrot Canvas", 
                                          width, height,
                                          SDL_WINDOW_RESIZABLE);


    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0); // creates renderer in window object with no other attributes, hence the 0

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Creation of texture given renderer object, width and height, other feilds give attributes to given texture 
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width, height);

    std::vector<uint32_t> pixels(width * height, 0); // Initiallize dynamically changing pixels 

    // Now the fun stuff, the running of the actual window... 

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) // User may exit at any time
                running = false;

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                width = event.window.data1;
                height = event.window.data2;
                pixels.assign(width * height, 0);

                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            width, height);
            }
        }

        // We update the texture with given dynamically allocated pixel vector 

        SDL_UpdateTexture(texture, nullptr, pixels.data(), width * sizeof(uint32_t));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }


    // Standard destructors on close 

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
