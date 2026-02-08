#include "main.h"
#include "types.h"
#include "text.h"

#define MAX_LINES 8
#define FONT_SIZE 48

text_t lines[MAX_LINES];
SDL_Rect lineLocation[MAX_LINES];

void linesInit() {
    memset(lines, 0, sizeof lines);
    memset(lineLocation, 0, sizeof lineLocation);
}

void createNewLine (const char *content, SDL_Rect rect, SDL_Surface *winSurface) {
    
    if (lines[MAX_LINES-1].used) {
        if (lines[MAX_LINES-1].font) TTF_CloseFont(lines[MAX_LINES-1].font);
    }

    for (int i = MAX_LINES - 1; i > 0; --i) {
        lines[i] = lines[i - 1];
    }

    // Clear bottom line in textbox
    memset(&lines[0], 0, sizeof lines[0]);

    lines[0].fontFile = "fonts/default.ttf";
    lines[0].fontSize = FONT_SIZE;
    lines[0].textColor = (SDL_Color){0, 0, 0, 255};
    lines[0].used = 1;
    lines[0].font = TTF_OpenFont(lines[0].fontFile, lines[0].fontSize);
    lines[0].textContent = content;

    updateTextbox(winSurface, rect);
}

void updateTextbox(SDL_Surface *winSurface, SDL_Rect rect) {
    int offset = FONT_SIZE;
    int line_x = 0;
    int line_y = rect.y - FONT_SIZE;

    int i;
    for (i=0; i < MAX_LINES; ++i) {
        text_t *t = &lines[i];
        if (t->used != 0) {
            // Create new surface with text on it
            SDL_Surface *textSurface = TTF_RenderUTF8_Solid(t->font, t->textContent, t->textColor);

            // Draw first line at bottom of textbox - set to top of textbox right now
            lineLocation[i].x = line_x;
            lineLocation[i].y = line_y + (i * offset);
            lineLocation[i].w = textSurface->w;
            lineLocation[i].h = textSurface->h;
       
        // Copy creates surface to window surface
        SDL_BlitSurface(textSurface, NULL, winSurface, &lineLocation[i]);
        SDL_FreeSurface(textSurface);
        }
    }
}

/*
void destroyText(text_t *t) {
    if (t->font) {
        TTF_CloseFont(t->font);
    }
    free(t->textContent);
    free(t);
}*/