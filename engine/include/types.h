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
    SDL_Color textColor;
    SDL_Texture *texture;
    uint8_t fontSize;
    bool used;
} text_t;

typedef struct coord_t {
    uint8_t x;
    uint8_t y;
} coord_t;

typedef struct player_t {
    char *name;
    coord_t coord;
} player_t;

typedef struct map_t {
    char *file;
    uint8_t data[64][64];
    coord_t size;
    coord_t start;
} map_t;

typedef struct background_t {
    SDL_Rect *location;
    SDL_Texture *texture;
    coord_t mapCoord;
    bool used;
} background_t;


#endif