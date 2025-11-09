#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <thread>

// Function to compute a portion of the Mandelbrot set (used for multithreading)
void computeMandelbrotSection(std::vector<uint32_t>& pixels, int width, int height,
                              double minRe, double maxRe, double minIm, double maxIm,
                              int maxIter, int yStart, int yEnd) {
    for (int y = yStart; y < yEnd; ++y) {
        double c_im = maxIm - y * (maxIm - minIm) / (height - 1);

        for (int x = 0; x < width; ++x) {
            double c_re = minRe + x * (maxRe - minRe) / (width - 1);
            double Z_re = c_re;
            double Z_im = c_im;

            int iter;
            for (iter = 0; iter < maxIter; ++iter) {
                double Z_re2 = Z_re * Z_re;
                double Z_im2 = Z_im * Z_im;

                if (Z_re2 + Z_im2 > 4.0)
                    break;

                double new_re = Z_re2 - Z_im2 + c_re;
                double new_im = 2.0 * Z_re * Z_im + c_im;
                Z_re = new_re;
                Z_im = new_im;
            }

            uint8_t color = uint8_t(255 * iter / maxIter);
            uint32_t pixelColor = 0xFF000000 | (color << 16) | (color << 8) | color;
            pixels[y * width + x] = pixelColor;
        }
    }
}

int main() {

    // to compile: 
    // g++ fractal.cpp -IC:\SDL3\include -LC:\SDL3\lib -lSDL3 -o fractal.exe

    // SDL INIT VIDEO: 
    // Initializes SDL video subsystem where you create windows, renderers and graphic displays 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // Sample width and height that are adjustable by SDL_WINDOW_RESIZABLE 
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

    // Creation of texture given renderer object, width and height, other fields give attributes to given texture 
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width, height);

    std::vector<uint32_t> pixels(width * height, 0); // Initialize dynamically changing pixels 

    // Flag to indicate whether the Mandelbrot set needs to be recalculated
    bool needsRedraw = true;

    // Now the fun stuff, the running of the actual window... 

    bool running = true;
    SDL_Event event;
    
    // Mandelbrot bounds and iteration count
    double minRe = -2.0;
    double maxRe = 1.0;
    double minIm = -1.2;
    double maxIm = minIm + (maxRe - minRe) * height / width; 
    int maxIter = 500; // Triple for loop for 500 iterations 

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
                
                needsRedraw = true; // Recalculate Mandelbrot on resize
            }

            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                double zoomFactor = (event.wheel.y > 0) ? 0.9 : 1.1; // scroll up to zoom in, down to zoom out

                // Get current mouse position in window coordinates
                float mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // now let's convert mouse coordinates to mathematically outputted equivalence
                double mouseRe = minRe + mouseX * (maxRe - minRe) / width; 
                double mouseIm = maxIm - mouseY * (maxIm - minIm) / height;

                // Adjust bounds to zoom relative to mouse
                double newWidth = (maxRe - minRe) * zoomFactor;
                double newHeight = (maxIm - minIm) * zoomFactor;

                minRe = mouseRe - newWidth * (mouseX / width);
                maxRe = minRe + newWidth;
                minIm = mouseIm - newHeight * (1.0 - mouseY / height);
                maxIm = minIm + newHeight;

                needsRedraw = true; // Recalculate Mandelbrot set after zoom
            }
        }

        if (needsRedraw) {
            // Multithreading setup: split the screen into sections
            unsigned int nThreads = std::thread::hardware_concurrency();
            if (nThreads == 0) nThreads = 4; // fallback if detection fails
            std::vector<std::thread> threads;

            int rowsPerThread = height / nThreads;

            for (unsigned int i = 0; i < nThreads; ++i) {
                int yStart = i * rowsPerThread;
                int yEnd = (i == nThreads - 1) ? height : yStart + rowsPerThread;

                threads.emplace_back(computeMandelbrotSection,
                                     std::ref(pixels),
                                     width, height,
                                     minRe, maxRe, minIm, maxIm,
                                     maxIter, yStart, yEnd);
            }

            for (auto& t : threads) t.join(); // wait for all threads to finish

            needsRedraw = false; // Reset redraw flag
        }

        // We update the texture with given dynamically allocated pixel vector 
        SDL_UpdateTexture(texture, nullptr, pixels.data(), width * sizeof(uint32_t));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(10); // Delay to make it easier on the CPU 
    }

    // Standard destructors on close 
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
