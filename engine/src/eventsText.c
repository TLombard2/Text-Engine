#include "eventsText.h"
#include <string.h>
#include <SDL2/SDL_ttf.h>


void eventTextInput(char *buffer, SDL_Event event) {
    if (strlen(buffer) + strlen(event.text.text) <= MAX_INPUT_LEN) {
        strcat(buffer, event.text.text);
    }
}


void eventTextBackspace(char *buffer, SDL_Event event) {
    size_t len = strlen(buffer);
    if (len > 0) {
        buffer[len - 1] = '\0';
    }
}


void eventTextReturn(char *buffer) {
    buffer[0] = '\0';
}