#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define MAX_MAP_SIZE 8204 // Allows 64x64 maps and includes header data

map_t map;
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

    // Parse file into individual lines
    char *line = strtok(mapBuffer, "\n");
    if (!line) return;

    // Store file name
    map.file = file;

    // First line - set size data
    sscanf(line, "%hhu %hhu", &map.sizeX, &map.sizeY); // hhu means uint8

    // Second line - set start coordinates
    line = strtok(NULL, "\n"); // NULL moves strtok forward 1 line
    if (!line) return;
    sscanf(line, "%hhu %hhu", &map.startX, &map.startY);

    // Third line - set map data
    memset(map.data, 0, sizeof(map.data)); // Clear map data

    // sizeY is map rows but is used here to define # of lines to iterate through the map data
    for (int y = 0; y < map.sizeY; y++) { 
        line = strtok(NULL, "\n"); // Get next line
        if (!line) break;

        // Copy the line here because strtok is already in use and you can't nest strtok calls
        char lineCopy[256];
        strncpy(lineCopy, line, sizeof(lineCopy) - 1);
        lineCopy[sizeof(lineCopy) - 1] = '\0'; // This guarantees a NUL Byte at the end to avoid reading past the buffer

        // Split row by spaces to get each tile value of the map
        char *token = strtok(lineCopy, " ");
        for (int x = 0; x < map.sizeX && token; x++) {
            map.data[y][x] = (uint8_t)atoi(token); // Convert string -> int
            token = strtok(NULL, " ");
        }
    }
}