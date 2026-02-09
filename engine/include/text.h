#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void linesInit();
void createNewLine(const char *content, SDL_Rect rect, SDL_Renderer *renderer);
void updateLinePositions(SDL_Rect rect);
void updateInputText(const char *content, SDL_Rect rect, SDL_Renderer *renderer);
void renderTextbox(SDL_Renderer *renderer, SDL_Rect rect);
void renderInput(SDL_Renderer *renderer, SDL_Rect rect);