#include <SDL2/SDL.h>
#include "types.h"

void initBackground(SDL_Rect *rect);
void createNewBackground(SDL_Renderer *renderer, const char *file);
void renderBackground(SDL_Renderer *renderer, SDL_Rect rect);