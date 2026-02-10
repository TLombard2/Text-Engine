#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

void setWindowSize (SDL_Window*);
void calcViewport(SDL_Window*, SDL_Rect*);
void calcTextbox(SDL_Window*, SDL_Rect*);

/* Resolve a relative asset path (e.g. "assets/fonts/default.ttf") to an
 * absolute path based on the executable's directory.  Returns a pointer
 * to a static buffer – overwritten on each call.  Works on both Linux
 * and Windows so the exe can live anywhere. */
const char *assetPath(const char *relative);