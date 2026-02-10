#ifndef MAPS_H
#define MAPS_H

#include <SDL2/SDL.h>
#include "types.h"

extern player_t player;

void createNewMap(char *file);
void printMap(void);
int *getMovementOptions (void);

#endif