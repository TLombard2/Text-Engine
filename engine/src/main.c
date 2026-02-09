#include "main.h"
#include "text.h"
#include <string.h>
#include "eventsText.h"

#define WINDOW_WIDTH 0
#define WINDOW_HEIGHT 0

void renderFrame(SDL_Renderer *renderer, SDL_Rect viewportRect, SDL_Rect textboxRect);

int main(int argc, char** args) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window* win = NULL;
    int window_flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP;

    win = SDL_CreateWindow("Text-Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
    setWindowSize(win);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Rect viewportRect = {};
    calcViewport(win, &viewportRect);
    SDL_Rect textboxRect = {};
    calcTextbox(win, &textboxRect);

    // Textbox Init
    linesInit();
    createNewLine("Hello World!", textboxRect, renderer);

    // Text Input Init
    SDL_StartTextInput();
    char inputBuffer[MAX_INPUT_LEN] = "";

    // Game loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
               (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
            if (event.type == SDL_TEXTINPUT) {
                eventTextInput(inputBuffer, event);
                if (inputBuffer[0] != '\0') {
                    updateInputText(inputBuffer, textboxRect, renderer);
                }
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE) {
                eventTextBackspace(inputBuffer, event);
                updateInputText(inputBuffer, textboxRect, renderer);
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN && strlen(inputBuffer) > 0) {
                    createNewLine(inputBuffer, textboxRect, renderer);
                    eventTextReturn(inputBuffer);
                    updateInputText(inputBuffer, textboxRect, renderer);
            }
        }

        renderFrame(renderer, viewportRect, textboxRect); // Render every frame - GPU doesn't recall prev frames
    }

    SDL_StopTextInput();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

void renderFrame(SDL_Renderer *renderer, SDL_Rect viewportRect, SDL_Rect textboxRect) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &viewportRect);

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderFillRect(renderer, &textboxRect);

    renderTextbox(renderer, textboxRect);
    renderInput(renderer, textboxRect);
    SDL_RenderPresent(renderer);
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



