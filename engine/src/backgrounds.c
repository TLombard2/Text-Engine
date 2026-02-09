#include <SDL2/SDL_image.h>
#include "backgrounds.h"

#define MAX_BACKGROUNDS 20

static SDL_Rect *sharedLocation = {0};
background_t backgrounds[MAX_BACKGROUNDS];

void initBackground(SDL_Rect *rect) {
    memset(backgrounds, 0, sizeof backgrounds);
    if (!sharedLocation) {
        sharedLocation = rect;
    }
}

void createNewBackground(SDL_Renderer *renderer, const char *file) {
    int i;
    for (i=0; i < MAX_BACKGROUNDS; ++i) {
        if (backgrounds[i].used == 0) {
            backgrounds[i].texture = IMG_LoadTexture(renderer, file);
            backgrounds[i].location = sharedLocation;
            break;
        } 
    }
}

void renderBackground(SDL_Renderer *renderer, SDL_Rect rect) { // Currently not using rect
    // Check current player coordinates

    SDL_RenderCopy(renderer, backgrounds[0].texture, NULL, backgrounds[0].location);
}