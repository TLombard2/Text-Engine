#include "main.h"
#include "types.h"
#include "text.h"

#define MAX_LINES 8
#define FONT_SIZE 48

static TTF_Font *sharedFont = NULL;

text_t lines[MAX_LINES];
SDL_Rect lineLocation[MAX_LINES];

void linesInit() {
    memset(lines, 0, sizeof lines);
    memset(lineLocation, 0, sizeof lineLocation);
    if (!sharedFont) {
        sharedFont = TTF_OpenFont("assets/fonts/default.ttf", FONT_SIZE);
    }
}

void createNewLine(const char *content, SDL_Rect rect, SDL_Renderer *renderer) {
    // Free top line's texture as it will be deleted
    if (lines[MAX_LINES-1].used) {
        if (lines[MAX_LINES-1].texture) SDL_DestroyTexture(lines[MAX_LINES-1].texture);
    }

    // Shift all lines up by one slot
    for (int i = MAX_LINES - 1; i > 0; --i) {
        lines[i] = lines[i - 1];
    }

    // Clear bottom line
    memset(&lines[0], 0, sizeof lines[0]);

    // Set new line
    lines[0].fontSize    = FONT_SIZE;
    lines[0].textColor   = (SDL_Color){0, 0, 0, 255};
    lines[0].used        = 1;
    lines[0].font        = sharedFont;
    lines[0].textContent = content;

    // Create GPU texture from rendered text
    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(lines[0].font, content, lines[0].textColor);
    lines[0].texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); // free surface - no longer needed

    // Reset line positions
    updateLinePositions(rect);
}

void updateLinePositions(SDL_Rect rect) {
    int offset = FONT_SIZE;
    for (int i = 0; i < MAX_LINES; ++i) {
        if (lines[i].used && lines[i].texture) {
            int texW, texH;
            SDL_QueryTexture(lines[i].texture, NULL, NULL, &texW, &texH);
            lineLocation[i].x = rect.x;
            lineLocation[i].y = (rect.y + rect.h - offset*2) - ((i + 1) * offset);
            lineLocation[i].w = texW;
            lineLocation[i].h = texH;
        }
    }
}

void renderTextbox(SDL_Renderer *renderer, SDL_Rect rect) {
    // Clip so text can't bleed into the viewport
    SDL_RenderSetClipRect(renderer, &rect);
    for (int i = 0; i < MAX_LINES; ++i) {
        if (lines[i].used && lines[i].texture) {
            // Only render if the line is within the textbox bounds
            if (lineLocation[i].y >= rect.y) {
                SDL_RenderCopy(renderer, lines[i].texture, NULL, &lineLocation[i]);
            }
        }
    }
    SDL_RenderSetClipRect(renderer, NULL); // reset clip
}

/// TEXT INPUT

static SDL_Texture *inputTexture = NULL;
static SDL_Rect inputLocation = {0};
static TTF_Font *inputFont = NULL;

void updateInputText(const char *content, SDL_Rect rect, SDL_Renderer *renderer) {
    if (inputTexture) {
        SDL_DestroyTexture(inputTexture);
        inputTexture = NULL;
    }

    if (!inputFont) {
        inputFont = sharedFont;
    }

    // If there is nothing to render return
    if (!content || content[0] == '\0') {
        memset(&inputLocation, 0, sizeof inputLocation);
        return;
    }

    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(inputFont, content, color);
    inputTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    inputLocation.x = rect.x + 8; // small margin
    inputLocation.y = (rect.y + rect.h) - FONT_SIZE - 8;
    inputLocation.w = textSurface->w;
    inputLocation.h = textSurface->h;

    SDL_FreeSurface(textSurface);
}

void renderInput(SDL_Renderer *renderer, SDL_Rect rect) {
    if (inputTexture) {
        SDL_RenderSetClipRect(renderer, &rect);
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputLocation);
        SDL_RenderSetClipRect(renderer, NULL); // Clips are persistent so must reset when done
    }
}