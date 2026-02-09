#include <SDL2/SDL.h>

#define MAX_INPUT_LEN 15

void eventTextInput(char *buffer, SDL_Event event);
void eventTextBackspace(char *buffer, SDL_Event event);
void eventTextReturn(char *buffer);