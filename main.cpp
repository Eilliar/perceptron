#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <math.h>

using namespace std;

const int SCREEN_WIDTH = 2*480; // Two canvas, sid
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 24; // Which yields a 20x20 "pixel" image, like the first
const int PERCEPTRON_SCREEN_WIDTH = 480;
const int PERCEPTRON_SCREEN_HEIGHT = 480;
const int ROWS = PERCEPTRON_SCREEN_HEIGHT / CELL_SIZE;
const int COLUMNS = PERCEPTRON_SCREEN_WIDTH / CELL_SIZE;
const int IMAGE_SIZE = SCREEN_HEIGHT / CELL_SIZE;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
vector<double> vImage(IMAGE_SIZE*IMAGE_SIZE, 0.0);

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

bool drawSquare(int r, int c, int w, int h){

    if(r > 20 || r < 0 || r+h > 20 || r+h < 0 || c > 20 || c < 0 || c+w > 20 || c+w < 0){
        return -1;
    }
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    int i = 0;
    fill(vImage.begin(), vImage.end(), 0);
    for(int row=r; row < r + h; row++){
        for(int col=c; col < c + w; col++, i++){
            SDL_Rect cellRect = { col * CELL_SIZE + 480, row * CELL_SIZE, w*CELL_SIZE, h*CELL_SIZE };
            SDL_RenderFillRect(gRenderer, &cellRect);
            vImage[IMAGE_SIZE * row + col] = 1;
        }
    }
    return 0;
}

bool inside_circle(int center_x, int center_y, int radius, int tile_x, int tile_y){
    float dx = center_x - tile_x;
    float dy = center_y - tile_y;
    float distance = sqrt(dx*dx + dy*dy);
    return distance <= radius;
}

bool drawCircle(int center_x, int center_y, int radius){

    if(center_x > 20 || center_x < 0 || center_x+radius > 20 || center_x - radius < 0){
        return -1;
    }
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    
    int i = 0;
    fill(vImage.begin(), vImage.end(), 0);
    for(int row=0; row<IMAGE_SIZE; row++){
        for(int col=0; col<IMAGE_SIZE; col++, i++){
            if(inside_circle(center_x, center_y, radius, col, row)){
                SDL_Rect cellRect = { col * CELL_SIZE + 480, row * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_RenderFillRect(gRenderer, &cellRect);
                vImage[i] = 1;
            }
        }
    }
    return 0;
}

// Used by figures
double rand_range(int lower=0, int upper=0){
    double r = ((double) rand() / (RAND_MAX));

    return r * (upper - lower) + lower;
}

// used by perceptron
double rand_init(){
    return ((double) rand() / (RAND_MAX));
}

int main(int argc, char* argv[]) {
    
    if (!init())
    {
        cout << "Failed to initialize SDL2!" << endl;;
        return -1;
    }

    srand((unsigned int)time(NULL)); // set random seed
    
    Uint64 lastElapsed = SDL_GetTicks64();
    SDL_Event event;
    bool quit = false;

    // Perceptron Initialization  weights
    vector<double> weights(IMAGE_SIZE*IMAGE_SIZE);
    generate(weights.begin(), weights.end(), rand_init);

    while (!quit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            }
        }

        Uint64 elapsed = SDL_GetTicks64();
        if(elapsed - lastElapsed >= 500){
            lastElapsed = elapsed;
            // Clear Canvas
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(gRenderer);

            // Draw Perceptron Weights
            int i = 0;
            double max_w = *max_element(weights.begin(), weights.end());
            double min_w = *min_element(weights.begin(), weights.end());
            for (int row = 0; row < ROWS; ++row) {
                for (int col = 0; col < COLUMNS; col++, i++) {
                    // int weight = (int) (weights[i] * 255);        
                    int weight = (int)  (255 * (weights[i] - min_w) / (max_w - min_w));
                    SDL_SetRenderDrawColor(gRenderer, weight, 128, 255, SDL_ALPHA_OPAQUE);
                    SDL_Rect cellRect = { col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                    SDL_RenderFillRect(gRenderer, &cellRect);   
                }
            }
            // Draw Image to be classified
            int error = 0;
            int y_target = 0;
            if((double) rand() / (RAND_MAX) > 0.5){
                // P
                error = drawCircle(rand_range(3, 16), rand_range(3, 16), rand_range(3, 6));
                y_target = 1;
            }
            else {
                // N
                error = drawSquare(rand_range(2, 17), rand_range(2, 17), rand_range(2, 6), rand_range(2, 6));
                y_target = 0;
            }

            if(error == 0) {
                double predict = 0;
                for(int i = 0; i < IMAGE_SIZE*IMAGE_SIZE; i++){
                    predict += weights[i] * vImage[i];
                }
                cout << "Dot Product : " << predict << "\tTarget: " << y_target;
                if(predict > 0) cout << "\tPrediction: 1" << endl;
                else cout << "\t Prediction: 0" << endl;
                // Update weights
                if(y_target == 1 && predict < 0){
                    transform(weights.begin(), weights.end(), vImage.begin(), weights.begin(), plus<double>( ));
                }
                if(y_target == 0 && predict >= 0){
                    transform(weights.begin(), weights.end(), vImage.begin(), weights.begin(), minus<double>( ));
                }
            }
        }

        SDL_RenderPresent(gRenderer);
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}
