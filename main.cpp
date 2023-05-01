#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 2*480;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 24; // Which yields a 20x20 "pixel" image, like the first perceptron
const int PERCEPTRON_SCREEN_WIDTH = 480;
const int PERCEPTRON_SCREEN_HEIGHT = 480;
const int ROWS = PERCEPTRON_SCREEN_HEIGHT / CELL_SIZE;
const int COLUMNS = PERCEPTRON_SCREEN_WIDTH / CELL_SIZE;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Perceptron", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr)
    {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr)
    {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    SDL_ShowCursor(true);
    return true;
}


int main(int argc, char* argv[]) {
    
    if (!init())
    {
        cout << "Failed to initialize SDL2!" << endl;;
        return -1;
    }

    srand((unsigned int)time(NULL)); // set random seed
    // Clear Canvas
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    // Draw the grid of cells
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLUMNS; ++col) {
            int weight = (int) (rand() * 255);
            SDL_SetRenderDrawColor(gRenderer, weight, 128, 255, SDL_ALPHA_OPAQUE);
            SDL_Rect cellRect = { col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_RenderFillRect(gRenderer, &cellRect);
        }
    }

    SDL_RenderPresent(gRenderer);

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}
