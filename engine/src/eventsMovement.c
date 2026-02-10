#include "eventsMovement.h"
#include "maps.h"

void movePlayer(SDL_Event event) {
    int *options = getMovementOptions();

    printf("sym=%d name=%s\n",
       event.key.keysym.sym,
       SDL_GetKeyName(event.key.keysym.sym));
       
    switch(event.key.keysym.sym) {
        case SDLK_RIGHT:
            if (options[0]== 1) {
                player.coord.x++;
            } break;
        case SDLK_LEFT:
            if (options[1]== 1) {
                player.coord.x--;
            } break;
        case SDLK_UP:
            if (options[2]== 1) {
                player.coord.y--;
            } break;
        case SDLK_DOWN:
            if (options[3]== 1) {
                player.coord.y++;
            } break;
    }
    printMap();
}