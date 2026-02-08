#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void setWindowSize (SDL_Window*);
void drawText();
void calcViewport(SDL_Window*, SDL_Rect*);
void calcTextbox(SDL_Window*, SDL_Rect*);