#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct text_t {
    TTF_Font *font;
    const char *textContent;
    SDL_Rect *location;
    const char *fontFile;
    SDL_Color textColor;
    SDL_Texture *texture;
    uint8_t fontSize;
    bool used;
} text_t;

#endif