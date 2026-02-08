#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void linesInit ();
void createNewLine (const char *content, SDL_Rect rect, SDL_Surface *winSurface);
void updateTextbox(SDL_Surface *winSurface, SDL_Rect rect);