/*  ============================================================
 *  Map Editor  –  raylib + raygui
 *  ============================================================
 *  Controls
 *  ─────────────────────────────────────────────────────────────
 *  Left-click map cell   → paint selected sprite tile
 *  Right-click map cell  → erase (set to 0)
 *  Shift + Left-click    → set player start position
 *  Mouse wheel over map  → zoom in / out
 *  F11                   → toggle fullscreen / windowed
 *  ESC                   → close current dialog / menu
 *  ============================================================ */

#include "raylib.h"

#include "main.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "filedialog.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ──────────────────────────────────────────────────────────────
 *  Global state
 * ────────────────────────────────────────────────────────────── */
static EditorMap   map         = {0};
static SpriteSheet sprite      = {0};

/* DPI scale – set once in main() */
float uiScale = 1.0f;

/* UI state */
static bool       fileMenuOpen = false;
static DialogType activeDialog = DIALOG_NONE;

/* New-map dialog fields */
static int  dialogMapW   = 10;
static int  dialogMapH   = 10;
static bool editDialogW  = false;
static bool editDialogH  = false;

/* Tile-size dialog fields (shown after picking a sprite file) */
static int  dialogTileW = 16;
static int  dialogTileH = 16;
static bool editTileW   = false;
static bool editTileH   = false;

/* Temporary sprite path (between load-sprite file pick → tile-size dialog) */
static char pendingSpritePath[MAX_PATH_LEN] = "";

/* Map-editor viewport */
static Vector2 mapScroll = {0, 0};
static int     cellSize  = DEFAULT_CELL_SIZE;

/* Sprite-panel viewport */
static Vector2 spriteScroll      = {0, 0};
static int     spriteDisplaySize = 48;

/* ──────────────────────────────────────────────────────────────
 *  Helpers
 * ────────────────────────────────────────────────────────────── */
static const char *baseFileName(const char *path)
{
    const char *p = strrchr(path, '/');
    if (!p) p = strrchr(path, '\\');
    return p ? p + 1 : path;
}

/* Draw one sprite-sheet tile into an arbitrary destination rect. */
static void drawSpriteTile(int tileIndex, Rectangle dest)
{
    if (!sprite.loaded || tileIndex <= 0) return;
    int idx = tileIndex - 1;
    int col = idx % sprite.cols;
    int row = idx / sprite.cols;
    Rectangle src = {
        (float)(col * sprite.tileWidth),
        (float)(row * sprite.tileHeight),
        (float)sprite.tileWidth,
        (float)sprite.tileHeight
    };
    DrawTexturePro(sprite.texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

/* ──────────────────────────────────────────────────────────────
 *  Save / Load  (extended map.txt format)
 *
 *  Line 1 : sprite_filename  tile_w  tile_h
 *  Line 2 : map_w  map_h
 *  Line 3 : start_x  start_y
 *  Lines 4+: space-separated tile values per row
 * ────────────────────────────────────────────────────────────── */
static void saveMap(const char *path)
{
    FILE *f = fopen(path, "w");
    if (!f) return;

    /* line 1 – sprite sheet info */
    fprintf(f, "%s %d %d\n",
            sprite.loaded ? sprite.fileName : "none",
            sprite.loaded ? sprite.tileWidth  : 0,
            sprite.loaded ? sprite.tileHeight : 0);

    /* line 2 – map dimensions */
    fprintf(f, "%d %d\n", map.width, map.height);

    /* line 3 – start position */
    fprintf(f, "%d %d\n", map.startX, map.startY);

    /* remaining lines – row data */
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            if (x > 0) fputc(' ', f);
            fprintf(f, "%d", map.data[y][x]);
        }
        fputc('\n', f);
    }

    fclose(f);
}

static bool loadMap(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) return false;

    char sprName[MAX_FILENAME_LEN] = {0};
    int  tileW = 0, tileH = 0;

    if (fscanf(f, "%255s %d %d", sprName, &tileW, &tileH) != 3) { fclose(f); return false; }
    if (fscanf(f, "%d %d", &map.width,  &map.height)      != 2) { fclose(f); return false; }
    if (fscanf(f, "%d %d", &map.startX, &map.startY)      != 2) { fclose(f); return false; }

    if (map.width  > MAX_MAP_WIDTH)  map.width  = MAX_MAP_WIDTH;
    if (map.height > MAX_MAP_HEIGHT) map.height = MAX_MAP_HEIGHT;

    memset(map.data, 0, sizeof(map.data));
    for (int y = 0; y < map.height; y++)
        for (int x = 0; x < map.width; x++)
            if (fscanf(f, "%d", &map.data[y][x]) != 1) break;

    fclose(f);

    strncpy(map.spriteFile, sprName, MAX_FILENAME_LEN - 1);
    map.spriteTileW = tileW;
    map.spriteTileH = tileH;
    map.active      = true;

    /* Try to auto-load the sprite sheet sitting next to the map file */
    if (strcmp(sprName, "none") != 0 && tileW > 0 && tileH > 0) {
        char sprPath[MAX_PATH_LEN];
        strncpy(sprPath, path, MAX_PATH_LEN - 1);
        char *slash = strrchr(sprPath, '/');
        if (slash)
            snprintf(slash + 1,
                     (size_t)(MAX_PATH_LEN - (slash - sprPath + 1)),
                     "%s", sprName);
        else
            strncpy(sprPath, sprName, MAX_PATH_LEN - 1);

        if (FileExists(sprPath)) {
            if (sprite.loaded) UnloadTexture(sprite.texture);
            sprite.texture    = LoadTexture(sprPath);
            sprite.tileWidth  = tileW;
            sprite.tileHeight = tileH;
            sprite.cols       = sprite.texture.width  / tileW;
            sprite.rows       = sprite.texture.height / tileH;
            sprite.totalTiles = sprite.cols * sprite.rows;
            sprite.selectedTile = 0;
            sprite.loaded     = true;
            strncpy(sprite.filePath, sprPath, MAX_PATH_LEN - 1);
            strncpy(sprite.fileName, sprName, MAX_FILENAME_LEN - 1);
        }
    }
    return true;
}

/* ──────────────────────────────────────────────────────────────
 *  Load a sprite sheet from disk
 * ────────────────────────────────────────────────────────────── */
static bool loadSpriteSheet(const char *path, int tileW, int tileH)
{
    if (!FileExists(path)) return false;

    if (sprite.loaded) UnloadTexture(sprite.texture);

    sprite.texture = LoadTexture(path);
    if (sprite.texture.id == 0) return false;

    sprite.tileWidth  = tileW;
    sprite.tileHeight = tileH;
    sprite.cols       = sprite.texture.width  / tileW;
    sprite.rows       = sprite.texture.height / tileH;
    sprite.totalTiles = sprite.cols * sprite.rows;
    sprite.selectedTile = 0;
    sprite.loaded     = true;

    strncpy(sprite.filePath, path, MAX_PATH_LEN - 1);
    strncpy(sprite.fileName, baseFileName(path), MAX_FILENAME_LEN - 1);

    /* Keep the map in sync */
    strncpy(map.spriteFile, sprite.fileName, MAX_FILENAME_LEN - 1);
    map.spriteTileW = tileW;
    map.spriteTileH = tileH;


    return true;
}

/* ──────────────────────────────────────────────────────────────
 *  Draw: Toolbar
 * ────────────────────────────────────────────────────────────── */
static void drawToolbar(int screenW)
{
    int tbH = S(TOOLBAR_HEIGHT);
    DrawRectangle(0, 0, screenW, tbH, (Color){40, 40, 40, 255});
    DrawLine(0, tbH, screenW, tbH, (Color){60, 60, 60, 255});

    /* ── File button ── */
    if (GuiButton((Rectangle){S(4), S(2), S(60), tbH - S(4)}, "File")) {
        fileMenuOpen = !fileMenuOpen;
    }

    /* ── Dropdown menu ── */
    if (fileMenuOpen) {
        int itemH = S(28);
        int menuW = S(150);
        int menuH = itemH * 4 + S(8);
        Rectangle menuBg = {S(4), tbH, menuW, menuH};
        DrawRectangleRec(menuBg, (Color){50, 50, 50, 255});
        DrawRectangleLinesEx(menuBg, 1, (Color){80, 80, 80, 255});

        int btnW = menuW - S(4);
        if (GuiButton((Rectangle){S(6), tbH + S(2),             btnW, itemH}, "  New Map")) {
            activeDialog = DIALOG_NEW_MAP;
            dialogMapW = 4;  dialogMapH = 4;
            editDialogW = editDialogH = false;
            fileMenuOpen = false;
        }
        if (GuiButton((Rectangle){S(6), tbH + S(2) + itemH + S(2), btnW, itemH}, "  Load Map")) {
            fileMenuOpen = false;
            char path[MAX_PATH_LEN] = {0};
            if (nativeFileDialog(path, MAX_PATH_LEN, 0, "Load Map", "Map files | *.txt")) {
                if (loadMap(path)) mapScroll = (Vector2){0, 0};
            }
        }
        if (GuiButton((Rectangle){S(6), tbH + S(2) + (itemH + S(2))*2, btnW, itemH}, "  Save Map")) {
            fileMenuOpen = false;
            if (map.active) {
                char path[MAX_PATH_LEN] = {0};
                if (nativeFileDialog(path, MAX_PATH_LEN, 1, "Save Map", "Map files | *.txt"))
                    saveMap(path);
            }
        }
        if (GuiButton((Rectangle){S(6), tbH + S(2) + (itemH + S(2))*3, btnW, itemH}, "  Load Sprite")) {
            fileMenuOpen = false;
            char path[MAX_PATH_LEN] = {0};
            if (nativeFileDialog(path, MAX_PATH_LEN, 0, "Load Sprite", "Images | *.png *.jpg *.bmp")) {
                ShowCursor();
                strncpy(pendingSpritePath, path, MAX_PATH_LEN - 1);
                dialogTileW = 16;  dialogTileH = 16;
                editTileW = editTileH = false;
                activeDialog = DIALOG_TILE_SIZE;
            }
        }
    }

    /* ── Info labels ── */
    int fontSize = S(14);
    int infoX = S(80);
    if (map.active) {
        const char *t = TextFormat("Map: %dx%d", map.width, map.height);
        DrawText(t, infoX, S(8), fontSize, LIGHTGRAY);
        infoX += MeasureText(t, fontSize) + S(20);
    }
    if (sprite.loaded) {
        const char *t = TextFormat("Sprite: %s  (%dx%d tiles)",
                                   sprite.fileName, sprite.cols, sprite.rows);
        DrawText(t, infoX, S(8), fontSize, LIGHTGRAY);
        infoX += MeasureText(t, fontSize) + S(20);
    }
    if (sprite.selectedTile > 0) {
        const char *t = TextFormat("Brush: tile %d", sprite.selectedTile);
        DrawText(t, infoX, S(8), fontSize, YELLOW);
    } else if (sprite.loaded) {
        DrawText("Brush: eraser", infoX, S(8), fontSize, (Color){200, 120, 120, 255});
    }

}

/* ──────────────────────────────────────────────────────────────
 *  Draw: Map Editor panel
 * ────────────────────────────────────────────────────────────── */
static void drawMapEditor(Rectangle area)
{
    DrawRectangleRec(area, (Color){30, 30, 30, 255});

    if (!map.active) {
        const char *msg = "No map loaded.  Use File > New Map or Load Map";
        int fs = S(18);
        int tw = MeasureText(msg, fs);
        DrawText(msg,
                 (int)(area.x + (area.width  - tw) / 2),
                 (int)(area.y + area.height / 2),
                 fs, GRAY);
        return;
    }

    /* Scrollable content size */
    Rectangle content = {
        0, 0,
        (float)(map.width  * cellSize + 20),
        (float)(map.height * cellSize + 20)
    };

    Rectangle view = {0};
    GuiScrollPanel(area, NULL, content, &mapScroll, &view);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    float ox = area.x + mapScroll.x + 10;
    float oy = area.y + mapScroll.y + 10;

    Vector2 mouse       = GetMousePosition();
    bool    canInteract = (activeDialog == DIALOG_NONE && !fileMenuOpen);

    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            Rectangle cell = {
                ox + x * cellSize,
                oy + y * cellSize,
                (float)cellSize,
                (float)cellSize
            };

            /* Frustum-cull tiles outside the visible scroll view */
            if (cell.x + cell.width  < view.x || cell.x > view.x + view.width  ||
                cell.y + cell.height < view.y || cell.y > view.y + view.height)
                continue;

            /* ── Cell background ── */
            int val = map.data[y][x];
            if (val > 0 && sprite.loaded) {
                drawSpriteTile(val, cell);
            } else if (val > 0) {
                DrawRectangleRec(cell, (Color){60, 100, 60, 255});
                const char *lbl = TextFormat("%d", val);
                int lblFs = S(10);
                int lw = MeasureText(lbl, lblFs);
                DrawText(lbl,
                         (int)(cell.x + (cell.width  - lw) / 2),
                         (int)(cell.y + (cell.height - lblFs) / 2),
                         lblFs, WHITE);
            } else {
                DrawRectangleRec(cell, (Color){20, 20, 20, 255});
            }

            /* ── Grid lines ── */
            DrawRectangleLinesEx(cell, 1, (Color){50, 50, 50, 255});

            /* ── Start-position marker ── */
            if (x == map.startX && y == map.startY) {
                DrawRectangleLinesEx(cell, 2, GREEN);
                DrawText("S", (int)cell.x + S(3), (int)cell.y + S(2), S(12), GREEN);
            }

            /* ── Hover / click ── */
            if (canInteract &&
                CheckCollisionPointRec(mouse, cell) &&
                CheckCollisionPointRec(mouse, view))
            {
                /* Highlight */
                DrawRectangleRec(cell, (Color){255, 255, 255, 40});

                if (IsKeyDown(KEY_LEFT_SHIFT) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    /* Shift+click → set start position */
                    map.startX = x;
                    map.startY = y;
                } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    /* Paint tile (held for drag-painting) */
                    map.data[y][x] = sprite.selectedTile;
                }

                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                    /* Erase */
                    map.data[y][x] = 0;
                }
            }
        }
    }

    EndScissorMode();

    /* ── Mouse-wheel zoom ── */
    if (canInteract && CheckCollisionPointRec(mouse, area)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            cellSize += (int)(wheel * S(4));
            if (cellSize < S(MIN_CELL_SIZE))  cellSize = S(MIN_CELL_SIZE);
            if (cellSize > S(MAX_CELL_SIZE))  cellSize = S(MAX_CELL_SIZE);
        }
    }

    /* ── Status line ── */
    if (canInteract && CheckCollisionPointRec(mouse, view)) {
        int hx = (int)((mouse.x - ox) / cellSize);
        int hy = (int)((mouse.y - oy) / cellSize);
        if (hx >= 0 && hx < map.width && hy >= 0 && hy < map.height) {
            const char *st = TextFormat("Cell (%d, %d)   Value: %d   Zoom: %d%%",
                                        hx, hy, map.data[hy][hx],
                                        (int)(cellSize * 100.0f / S(DEFAULT_CELL_SIZE)));
            DrawText(st,
                     (int)area.x + S(8),
                     (int)(area.y + area.height - S(22)),
                     S(14), LIGHTGRAY);
        }
    }
}

/* ──────────────────────────────────────────────────────────────
 *  Draw: Sprite panel  (right-hand side)
 * ────────────────────────────────────────────────────────────── */
static void drawSpritePanel(Rectangle area)
{
    int headerH = S(24);
    /* Header bar */
    DrawRectangle((int)area.x, (int)area.y, (int)area.width, headerH,
                  (Color){40, 40, 40, 255});
    DrawText("Sprites", (int)area.x + S(8), (int)area.y + S(4), S(16), LIGHTGRAY);

    /* Reserve space for the eraser button at the bottom */
    float eraserH = S(32);
    Rectangle eraserBtn = {
        area.x + S(8), area.y + area.height - eraserH - S(4),
        area.width - S(16), eraserH
    };

    Rectangle panelBody = {
        area.x, area.y + headerH,
        area.width, area.height - headerH - eraserH - S(12)
    };
    DrawRectangleRec(panelBody, (Color){35, 35, 35, 255});

    if (!sprite.loaded) {
        DrawText("No sprite loaded.",   (int)area.x + S(12), (int)area.y + S(50), S(14), GRAY);
        DrawText("File > Load Sprite",  (int)area.x + S(12), (int)area.y + S(70), S(14), GRAY);
        if (GuiButton(eraserBtn, "Eraser (Clear Tile)")) sprite.selectedTile = 0;
        return;
    }

    /* Grid layout for sprite tiles */
    int pad         = S(4);
    int tilesPerRow = (int)((panelBody.width - S(20)) / (spriteDisplaySize + pad));
    if (tilesPerRow < 1) tilesPerRow = 1;
    int totalRows   = (sprite.totalTiles + tilesPerRow - 1) / tilesPerRow;

    Rectangle content = {
        0, 0,
        (float)(tilesPerRow * (spriteDisplaySize + pad) + S(20)),
        (float)(totalRows   * (spriteDisplaySize + pad) + S(20))
    };

    Rectangle view = {0};
    GuiScrollPanel(panelBody, NULL, content, &spriteScroll, &view);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    Vector2 mouse       = GetMousePosition();
    bool    canInteract = (activeDialog == DIALOG_NONE && !fileMenuOpen);

    for (int i = 0; i < sprite.totalTiles; i++) {
        int gc = i % tilesPerRow;
        int gr = i / tilesPerRow;

        Rectangle dest = {
            panelBody.x + spriteScroll.x + S(10) + gc * (spriteDisplaySize + pad),
            panelBody.y + spriteScroll.y + S(10) + gr * (spriteDisplaySize + pad),
            (float)spriteDisplaySize,
            (float)spriteDisplaySize
        };

        if (dest.x + dest.width  < view.x || dest.x > view.x + view.width  ||
            dest.y + dest.height < view.y || dest.y > view.y + view.height)
            continue;

        /* Background */
        DrawRectangleRec(dest, (Color){25, 25, 25, 255});

        /* Draw the actual sprite tile from the sheet */
        int srcCol = i % sprite.cols;
        int srcRow = i / sprite.cols;
        Rectangle src = {
            (float)(srcCol * sprite.tileWidth),
            (float)(srcRow * sprite.tileHeight),
            (float)sprite.tileWidth,
            (float)sprite.tileHeight
        };
        DrawTexturePro(sprite.texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);

        /* Selection highlight */
        int tileIdx = i + 1;   /* 1-based index */
        if (tileIdx == sprite.selectedTile)
            DrawRectangleLinesEx(dest, 3, YELLOW);
        else
            DrawRectangleLinesEx(dest, 1, (Color){60, 60, 60, 255});

        /* Hover + click */
        if (canInteract &&
            CheckCollisionPointRec(mouse, dest) &&
            CheckCollisionPointRec(mouse, view))
        {
            DrawRectangleRec(dest, (Color){255, 255, 255, 30});
            DrawText(TextFormat("%d", tileIdx),
                     (int)dest.x + S(2), (int)dest.y + S(2), S(10), WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                sprite.selectedTile = tileIdx;
        }
    }

    EndScissorMode();

    /* Eraser button */
    if (GuiButton(eraserBtn, "Eraser (Clear Tile)"))
        sprite.selectedTile = 0;
}

/* ──────────────────────────────────────────────────────────────
 *  Draw: Dialogs  (rendered last — on top of everything)
 * ────────────────────────────────────────────────────────────── */
static void drawDialogs(int screenW, int screenH)
{
    if (activeDialog == DIALOG_NONE) return;

    /* Dim background */
    DrawRectangle(0, 0, screenW, screenH, (Color){0, 0, 0, 120});

    float cx = screenW  / 2.0f;
    float cy = screenH  / 2.0f;

    switch (activeDialog) {

    /* ─── New Map ─── */
    case DIALOG_NEW_MAP: {
        Rectangle win = {cx - S(170), cy - S(110), S(340), S(220)};
        if (GuiWindowBox(win, "New Map")) { activeDialog = DIALOG_NONE; break; }

        GuiLabel((Rectangle){win.x + S(20), win.y + S(40), S(100), S(24)}, "Width:");
        if (GuiSpinner((Rectangle){win.x + S(120), win.y + S(40), S(180), S(30)},
                       NULL, &dialogMapW, 1, MAX_MAP_WIDTH, editDialogW))
            editDialogW = !editDialogW;

        GuiLabel((Rectangle){win.x + S(20), win.y + S(80), S(100), S(24)}, "Height:");
        if (GuiSpinner((Rectangle){win.x + S(120), win.y + S(80), S(180), S(30)},
                       NULL, &dialogMapH, 1, MAX_MAP_HEIGHT, editDialogH))
            editDialogH = !editDialogH;

        if (GuiButton((Rectangle){win.x + S(40),  win.y + S(150), S(110), S(36)}, "Create")) {
            memset(&map, 0, sizeof(map));
            map.width  = dialogMapW;
            map.height = dialogMapH;
            map.active = true;
            mapScroll  = (Vector2){0, 0};
            activeDialog = DIALOG_NONE;
        }
        if (GuiButton((Rectangle){win.x + S(190), win.y + S(150), S(110), S(36)}, "Cancel"))
            activeDialog = DIALOG_NONE;
    } break;

    /* ─── Tile Size  (after picking sprite file via file explorer) ─── */
    case DIALOG_TILE_SIZE: {
        Rectangle win = {cx - S(180), cy - S(110), S(360), S(220)};
        if (GuiWindowBox(win, "Sprite Tile Size (pixels)")) { activeDialog = DIALOG_NONE; break; }

        GuiLabel((Rectangle){win.x + S(20), win.y + S(40), S(130), S(24)}, "Tile Width:");
        if (GuiSpinner((Rectangle){win.x + S(160), win.y + S(40), S(160), S(30)},
                       NULL, &dialogTileW, 1, 512, editTileW))
            editTileW = !editTileW;

        GuiLabel((Rectangle){win.x + S(20), win.y + S(80), S(130), S(24)}, "Tile Height:");
        if (GuiSpinner((Rectangle){win.x + S(160), win.y + S(80), S(160), S(30)},
                       NULL, &dialogTileH, 1, 512, editTileH))
            editTileH = !editTileH;

        if (GuiButton((Rectangle){win.x + S(40),  win.y + S(150), S(120), S(36)}, "Load")) {
            loadSpriteSheet(pendingSpritePath, dialogTileW, dialogTileH);
            spriteScroll = (Vector2){0, 0};
            activeDialog = DIALOG_NONE;
        }
        if (GuiButton((Rectangle){win.x + S(200), win.y + S(150), S(120), S(36)}, "Cancel"))
            activeDialog = DIALOG_NONE;
    } break;

    default: break;
    }
}

/* ──────────────────────────────────────────────────────────────
 *  Close the file menu when clicking outside it
 * ────────────────────────────────────────────────────────────── */
static void handleFileMenuDismiss(void)
{
    if (!fileMenuOpen) return;
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    Vector2   m       = GetMousePosition();
    int tbH = S(TOOLBAR_HEIGHT);
    Rectangle fileBtn = {S(4), S(2), S(60), tbH - S(4)};
    int itemH = S(28);
    Rectangle menu    = {S(4), tbH, S(150), itemH * 4 + S(8)};

    if (!CheckCollisionPointRec(m, fileBtn) &&
        !CheckCollisionPointRec(m, menu))
        fileMenuOpen = false;
}

/* ──────────────────────────────────────────────────────────────
 *  Entry point
 * ────────────────────────────────────────────────────────────── */
int main(void)
{
    /* ── Window: resizable, starts maximized (not fullscreen).       ──
     * Avoids WSL/X11 issues where GetMonitorWidth/Height return wrong  
     * values and ToggleFullscreen creates a mismatched framebuffer.    */
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(1920, 1080, "Map Editor");

    /* Initial UI scale from current window (recomputed each frame) */
    uiScale = (float)GetScreenHeight() / 1080.0f;
    if (uiScale < 0.5f) uiScale = 0.5f;

    cellSize          = S(DEFAULT_CELL_SIZE);
    spriteDisplaySize = S(48);

    SetTargetFPS(60);
    SetExitKey(0);   /* ESC is used for closing menus, not quitting */

    GuiSetStyle(DEFAULT, TEXT_SIZE, S(14));

    /* Ensure cursor is visible (WSL/X11 can hide it on init) */
    ShowCursor();

    /* ── Main loop ── */
    while (!WindowShouldClose()) {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        /* ── Recompute UI scale each frame so resizing / WSL works ── */
        float newScale = (float)screenH / 1080.0f;
        if (newScale < 0.5f) newScale = 0.5f;
        if (fabsf(newScale - uiScale) > 0.001f) {
            uiScale = newScale;
            cellSize          = S(DEFAULT_CELL_SIZE);
            spriteDisplaySize = S(48);
            GuiSetStyle(DEFAULT, TEXT_SIZE, S(14));
        }

        /* ── Global keys ── */
        if (IsKeyPressed(KEY_ESCAPE)) {
            if      (activeDialog != DIALOG_NONE) activeDialog = DIALOG_NONE;
            else if (fileMenuOpen)                fileMenuOpen = false;
        }

        /* Dismiss file menu on outside click */
        if (activeDialog == DIALOG_NONE) handleFileMenuDismiss();

        /* ── Draw ── */
        BeginDrawing();
        ClearBackground((Color){25, 25, 25, 255});

        /* Layout */
        float spW = S(SPRITE_PANEL_WIDTH);
        float tbH = S(TOOLBAR_HEIGHT);
        Rectangle mapArea = {
            0, tbH,
            screenW - spW, screenH - tbH
        };
        Rectangle spriteArea = {
            screenW - spW, tbH,
            spW, screenH - tbH
        };

        /* Divider */
        DrawLine((int)spriteArea.x, (int)spriteArea.y,
                 (int)spriteArea.x, (int)(spriteArea.y + spriteArea.height),
                 (Color){60, 60, 60, 255});

        /* Panels */
        drawMapEditor(mapArea);
        drawSpritePanel(spriteArea);

        /* Toolbar (on top so its dropdown overlaps panels) */
        drawToolbar(screenW);

        /* Dialogs (topmost layer) */
        drawDialogs(screenW, screenH);

        EndDrawing();
    }

    /* ── Cleanup ── */
    if (sprite.loaded) UnloadTexture(sprite.texture);
    CloseWindow();
    return 0;
}