#include <SDL2/SDL_image.h>
#include "backgrounds.h"

#define MAX_BACKGROUNDS 20

static SDL_Rect *sharedLocation = {0};
background_t background;

void initBackground(SDL_Rect *rect) {
    if (!sharedLocation) {
        sharedLocation = rect;
    }
}

void createNewBackground(SDL_Renderer *renderer, const char *file) {
    
    if (background.used == 0) {
        background.texture = IMG_LoadTexture(renderer, file);
        background.location = sharedLocation;
    } 
}

void renderBackground(SDL_Renderer *renderer, SDL_Rect rect) { // Currently not using rect
    SDL_RenderCopy(renderer, background.texture, NULL, background.location);
}