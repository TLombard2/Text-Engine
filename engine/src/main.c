#include "main.h"
#include "text.h"
#define WINDOW_WIDTH 0
#define WINDOW_HEIGHT 0

int main(int argc, char** args) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window* win = NULL;
    SDL_Surface* winSurface = NULL;
    int window_flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP;

    win = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
    
    // Documentation says Fullscreen flag ignores w,h but the surface doesn't so it is set again here.
    setWindowSize(win);
   
    winSurface = SDL_GetWindowSurface(win);
    SDL_Rect viewportRect = {};
    calcViewport(win, &viewportRect);
    SDL_Rect textboxRect ={};
    calcTextbox(win, &textboxRect);

    SDL_FillRect(winSurface, &viewportRect, SDL_MapRGB(winSurface->format, 255, 255, 255));
    SDL_FillRect(winSurface, &textboxRect, SDL_MapRGB(winSurface->format, 180, 180, 180));

    linesInit(); // from text.c

    const char *newText = "Hello World!";
    createNewLine(newText, textboxRect, winSurface); // from text.c
    SDL_Delay(1000);
    newText = "TEST LINE 2!!";
    createNewLine(newText, textboxRect, winSurface); // from text.c

    SDL_UpdateWindowSurface(win);

    SDL_Delay(5000);
    TTF_Quit();
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

void setWindowSize(SDL_Window *win) {
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    SDL_SetWindowSize(win, w, h);
}

void calcViewport(SDL_Window *win, SDL_Rect *rect) {
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    rect->x = 0;
    rect->y = 0;
    rect->w = w;
    rect->h = (h / 4) * 3;
}

void calcTextbox(SDL_Window *win, SDL_Rect *rect) {
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    rect->x = 0;
    rect->y = (h / 4) * 3;
    rect->w = w;
    rect->h = (h / 4);
}



