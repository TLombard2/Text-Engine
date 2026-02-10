#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maps.h"

#define MAX_MAP_SIZE 8204 // Allows 64x64 maps and includes header data

map_t map;
player_t player;
static char mapBuffer[MAX_MAP_SIZE];

void createNewMap(char *file) {
    // Open map file
    SDL_RWops *mapFile = SDL_RWFromFile(file, "r");
    if (!mapFile) {
        fprintf(stderr, "SDL_RWFromFile('%s') failed: %s\n", file, SDL_GetError());
        return;
    }

    // Check file size
    int totalSize = SDL_RWsize(mapFile);
    if (totalSize <= 0 || totalSize >= MAX_MAP_SIZE) {
        fprintf(stderr, "Map too large or empty: %d\n", totalSize);
        SDL_RWclose(mapFile);
        return;
    }


    // Store file in buffer and close file
    size_t bytesRead = SDL_RWread(mapFile, mapBuffer, 1, totalSize);
    SDL_RWclose(mapFile);
    mapBuffer[bytesRead] = '\0';

    // Parse file into individual lines using strtok_r (reentrant) so we can
    // safely tokenize lines and tokens within lines without conflicts.
    char *lineSave = NULL; // strtok_r state for line splitting
    char *line = strtok_r(mapBuffer, "\n", &lineSave);
    if (!line) return;

    // Store file name
    map.file = file;

    // First line - sprite sheet info: "sprite.png tileW tileH"
    memset(map.spriteName, 0, sizeof(map.spriteName));
    map.spriteTileW = 0;
    map.spriteTileH = 0;
    sscanf(line, "%255s %hu %hu", map.spriteName, &map.spriteTileW, &map.spriteTileH);

    // Second line - set size data
    line = strtok_r(NULL, "\n", &lineSave);
    if (!line) return;
    sscanf(line, "%hhu %hhu", &map.size.x, &map.size.y); // hhu means uint8

    // Third line - set start coordinates
    line = strtok_r(NULL, "\n", &lineSave);
    if (!line) return;
    sscanf(line, "%hhu %hhu", &map.start.x, &map.start.y);

    // Set player location to the start
    player.coord.x = map.start.x;
    player.coord.y = map.start.y;

    // Third line - set map data
    memset(map.data, 0, sizeof(map.data)); // Clear map data

    // sizeY is map rows but is used here to define # of lines to iterate through the map data
    for (int y = 0; y < map.size.y; y++) { 
        line = strtok_r(NULL, "\n", &lineSave); // Get next line
        if (!line) break;

        // Split row by spaces to get each tile value of the map
        char *tokenSave = NULL; // separate strtok_r state for token splitting
        char *token = strtok_r(line, " ", &tokenSave);
        for (int x = 0; x < map.size.x && token; x++) {
            map.data[y][x] = (uint8_t)atoi(token);
            token = strtok_r(NULL, " ", &tokenSave);
        }
    }
}

void printMap() {
    printf("\n=== Map Data (size: %d x %d) ===\n", map.size.x, map.size.y);
    for (int y = 0; y < map.size.y; y++) {
        for (int x = 0; x < map.size.x; x++) {
            if (x == player.coord.x && y == player.coord.y) {
                printf(" P"); // Mark player position
            } else {
                printf(" %d", map.data[y][x]);
            }
        }
        printf("\n");
    }
    printf("Player at: (%d, %d)\n", player.coord.x, player.coord.y);
    printf("===========================\n\n");
}

int *getMovementOptions () {
    static int options[4]; // right, left, top, bottom
    memset(options, 0, sizeof(options));

    int px = player.coord.x;
    int py = player.coord.y;

    if (px != map.size.x - 1) { // Not on right edge
        int right = map.data[py][px + 1];
        if (right > 0) {
            options[0] = 1;
        }
    }
    if (px != 0) { // Not on left edge
        int left = map.data[py][px - 1];
        if (left > 0) {
            options[1] = 1;
        }
    }
    if (py != 0) { // Not on top edge
        int top = map.data[py - 1][px];
        if (top > 0) {
            options[2] = 1;
        }
    }
    if (py != map.size.y - 1) { // Not on bottom edge
        int bot = map.data[py + 1][px];
        if (bot > 0) {
            options[3] = 1;
        }
    }

    printf("OPTIONS %d %d %d %d", options[0], options[1], options[2], options[3]);
    return options;
}